#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

// blep https://www.taletn.com/reaper/mono_synth/
// dsf https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html

using namespace juce::dsp;

FFOsciProcessor::
FFOsciProcessor() :
_oversampler(
  FFOsciUniMaxCount, FFOsciOversampleFactor,
  Oversampling<float>::filterHalfBandPolyphaseIIR, false, false)
{
  _oversampler.initProcessing(FBFixedBlockSamples);
  for (int u = 0; u < FFOsciUniMaxCount; u++)
    _downsampledChannelPtrs[u] = _uniOutput[u].Ptr(0);
  _downsampledBlock = AudioBlock<float>(_downsampledChannelPtrs.data(), FFOsciUniMaxCount, 0, FBFixedBlockSamples);
  _oversampledBlock = _oversampler.processSamplesUp(_downsampledBlock);
}

void
FFOsciProcessor::InitializeBuffers(bool graph, float sampleRate)
{
  int oversampleTimes = graph ? 1 : FFOsciOversampleTimes;
  int maxDelayLineSize = static_cast<int>(std::ceil(sampleRate * oversampleTimes / FFOsciStringMinFreq));
  for (int i = 0; i < FFOsciUniMaxCount; i++)
    _stringUniState[i].delayLine.InitializeBuffers(maxDelayLineSize);
}

void
FFOsciProcessor::BeginVoice(bool graph, FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  auto const& typeNorm = params.block.type[0].Voice()[voice];
  _type = topo.NormalizedToListFast<FFOsciType>(FFOsciParam::Type, typeNorm);
  if (_type == FFOsciType::Off)
    return;

  int modStartSlot = OsciModStartSlot(state.moduleSlot);
  auto const& modParams = procState->param.voice.osciMod[0];
  auto const& modTopo = state.topo->static_.modules[(int)FFModuleType::OsciMod];

  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];
  auto const& uniOffsetNorm = params.block.uniOffset[0].Voice()[voice];
  auto const& uniRandomNorm = params.block.uniRandom[0].Voice()[voice];
  auto const& modExpoFMNorm = modParams.block.expoFM[0].Voice()[voice];
  auto const& modOversampleNorm = modParams.block.oversample[0].Voice()[voice];

  _graph = graph;
  _phaseGen = {};
  _key = static_cast<float>(state.voice->event.note.key);
  _uniformPrng = FFParkMillerPRNG(state.moduleSlot / static_cast<float>(FFOsciCount));
  bool oversample = modTopo.NormalizedToBoolFast(FFOsciModParam::Oversample, modOversampleNorm);
  _oversampleTimes = (!graph && oversample) ? FFOsciOversampleTimes : 1;

  _uniCount = topo.NormalizedToDiscreteFast(FFOsciParam::UniCount, uniCountNorm);
  _uniOffsetPlain = topo.NormalizedToIdentityFast(FFOsciParam::UniOffset, uniOffsetNorm);
  _uniRandomPlain = topo.NormalizedToIdentityFast(FFOsciParam::UniRandom, uniRandomNorm);

  FBSArray<float, FFOsciUniMaxCount> uniPhaseInit = {};
  for (int u = 0; u < _uniCount; u++)
  {
    if (_uniCount == 1)
    {
      _uniPosMHalfToHalf.Set(u, 0.0f);
      _uniPosAbsHalfToHalf.Set(u, 0.0f);
    }
    else
    {
      _uniPosMHalfToHalf.Set(u, u / (_uniCount - 1.0f) - 0.5f);
      _uniPosAbsHalfToHalf.Set(u, std::fabs(_uniPosMHalfToHalf.Get(u)));
    }
    float uniPhase = u * _uniOffsetPlain / _uniCount;
    uniPhaseInit.Set(u, ((1.0f - _uniRandomPlain) + _uniRandomPlain * _uniformPrng.NextScalar()) * uniPhase);
  }

  _modMatrixExpoFM = modTopo.NormalizedToBoolFast(FFOsciModParam::ExpoFM, modExpoFMNorm);
  for (int modSlot = modStartSlot; modSlot < modStartSlot + state.moduleSlot; modSlot++)
  {
    int srcOsciSlot = modSlot - modStartSlot;
    auto const& srcOsciParams = procState->param.voice.osci[srcOsciSlot];
    auto const& modFMOnNorm = modParams.block.fmOn[modSlot].Voice()[voice];
    auto const& modAMModeNorm = modParams.block.amMode[modSlot].Voice()[voice];
    auto const& modUniCountNorm = srcOsciParams.block.uniCount[0].Voice()[voice];
    _modSourceFMOn[srcOsciSlot] = modTopo.NormalizedToBoolFast(FFOsciModParam::FMOn, modFMOnNorm);
    _modSourceUniCount[srcOsciSlot] = topo.NormalizedToDiscreteFast(FFOsciParam::UniCount, modUniCountNorm);
    _modSourceAMMode[srcOsciSlot] = modTopo.NormalizedToListFast<FFOsciModAMMode>(FFOsciModParam::AMMode, modAMModeNorm);
  }

  if (_type == FFOsciType::Wave)
    BeginVoiceWave(state, uniPhaseInit);
  else if (_type == FFOsciType::FM)
    BeginVoiceFM(state, uniPhaseInit);
  else if (_type == FFOsciType::String)
    BeginVoiceString(graph, state);
  else
    FB_ASSERT(false);
}

int
FFOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.osci[state.moduleSlot].output;
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  output.Fill(0.0f);
  uniOutputOversampled.Fill(0.0f);
  if (_type == FFOsciType::Off)
    return 0;

  float sampleRate = state.input->sampleRate;
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  int prevPositionSamplesUpToFirstCycle = _phaseGen.PositionSamplesUpToFirstCycle();

  auto const& panNorm = procParams.acc.pan[0].Voice()[voice];
  auto const& gainNorm0 = procParams.acc.gain[0].Voice()[voice]; // TODO TEMP
  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.uniBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.uniDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.uniSpread[0].Voice()[voice];

  // TODO TEMP - add vlfo1 to osc1 gain and glfo1 to osc2 gain
  FBSArray<float, FBFixedBlockSamples> gainNorm;
  for (int i = 0; i < FBFixedBlockSamples; i++)
    gainNorm.Set(i, gainNorm0.CV().Get(i));
  if (state.moduleSlot == 0 && !_graph)
    for (int i = 0; i < FBFixedBlockSamples; i++)
      gainNorm.Set(i, std::clamp(gainNorm.Get(i) + voiceState.vLFO[0].output.Get(i), 0.0f, 1.0f));
  if (state.moduleSlot == 1 && !_graph)
    for (int i = 0; i < FBFixedBlockSamples; i++)
      gainNorm.Set(i, std::clamp(gainNorm.Get(i) + procState->dsp.global.gLFO[0].output.Get(i), 0.0f, 1.0f));

  FBSArray<float, FFOsciFixedBlockOversamples> panPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> gainPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> uniBlendPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> uniSpreadPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> uniDetunePlain;
  FBSArray<float, FFOsciFixedBlockOversamples> baseFreqPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> basePitchPlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm, s);
    auto coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, s);
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    basePitchPlain.Store(s, pitch);
    baseFreqPlain.Store(s, baseFreq);
    _phaseGen.NextBatch(baseFreq / sampleRate);

    panPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::Pan, panNorm, s));
    gainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::Gain, gainNorm.Load(s))); // TODO temp
    uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniBlend, uniBlendNorm, s));
    uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniSpread, uniSpreadNorm, s));
    uniDetunePlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniDetune, uniDetuneNorm, s));
  }
  if (_oversampleTimes != 1)
  {
    uniDetunePlain.UpsampleStretch<FFOsciOversampleTimes>();
    baseFreqPlain.UpsampleStretch<FFOsciOversampleTimes>();
    basePitchPlain.UpsampleStretch<FFOsciOversampleTimes>();
  }

  if (_type == FFOsciType::Wave)
    ProcessWave(state, basePitchPlain, uniDetunePlain);
  else if (_type == FFOsciType::FM)
    ProcessFM(state, basePitchPlain, uniDetunePlain);
  else if (_type == FFOsciType::String)
    ProcessString(state, basePitchPlain, uniDetunePlain);
  else
    FB_ASSERT(false);

  for (int u = 0; u < _uniCount; u++)
    for (int src = 0; src < state.moduleSlot; src++)
      if (_modSourceAMMode[src] != FFOsciModAMMode::Off && _modSourceUniCount[src] > u)
      {
        float scale = _modSourceAMMode[src] == FFOsciModAMMode::RM ? 1.0f : 0.5f;
        float offset = _modSourceAMMode[src] == FFOsciModAMMode::RM ? 0.0f : 0.5f;
        for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
        {
          int modSlot = OsciModStartSlot(state.moduleSlot) + src;
          auto outputAMMix = voiceState.osciMod.outputAMMix[modSlot].Load(s);
          auto thisUniOutput = uniOutputOversampled[u].Load(s);
          auto const& thatUniOutput = voiceState.osci[src].uniOutputOversampled[u].Load(s);
          uniOutputOversampled[u].Store(s, (1.0f - outputAMMix) * thisUniOutput + outputAMMix * thisUniOutput * (thatUniOutput * scale + offset));
        }
      }

  if (_oversampleTimes == 1)
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        _uniOutput[u].Store(s, uniOutputOversampled[u].Load(s));
  else
  {
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FFOsciFixedBlockOversamples; s++)
        _oversampledBlock.setSample(u, s, uniOutputOversampled[u].Get(s));
    auto channelBlockDown = _downsampledBlock.getSubsetChannelBlock(0, _uniCount);
    _oversampler.processSamplesDown(channelBlockDown);
  }

  for (int u = 0; u < _uniCount; u++)
  {
    float uniPosMHalfToHalf = _uniPosMHalfToHalf.Get(u);
    float uniPosAbsHalfToHalf = _uniPosAbsHalfToHalf.Get(u);
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      auto panning = panPlain.Load(s);
      auto uniPanning = 0.5f + uniPosMHalfToHalf * uniSpreadPlain.Load(s);
      auto uniBlend = 1.0f - (uniPosAbsHalfToHalf * 2.0f * (1.0f - uniBlendPlain.Load(s)));
      auto uniMono = _uniOutput[u].Load(s) * gainPlain.Load(s) * uniBlend;
      output[0].Add(s, (1.0f - uniPanning) * uniMono * xsimd::sqrt(1.0f - panning));
      output[1].Add(s, uniPanning * uniMono * xsimd::sqrt(panning));
    }
  }
  output.NaNCheck();

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  float lastBaseFreq = baseFreqPlain.Get(_oversampleTimes * FBFixedBlockSamples - 1);
  if (exchangeToGUI == nullptr)
  {
    if (_type == FFOsciType::String)
    {
      int graphSamples = FBFreqToSamples(lastBaseFreq, sampleRate) * FFOsciStringGraphRounds;
      return std::clamp(graphSamples - _stringGraphPosition, 0, FBFixedBlockSamples);
    }
    else
      return _phaseGen.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;
  }

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(lastBaseFreq, state.input->sampleRate);

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.pan[0][voice] = panNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainNorm.Last();
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.uniBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.uniDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.uniSpread[0][voice] = uniSpreadNorm.Last();
  return FBFixedBlockSamples;
}