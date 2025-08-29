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
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

#include <libMTSClient.h>
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
FFOsciProcessor::ReleaseOnDemandBuffers(
  FBRuntimeTopo const*, FBProcStateContainer* state)
{
  for (int i = 0; i < FFOsciUniMaxCount; i++)
    _stringUniState[i].delayLine.ReleaseBuffers(state->MemoryPool());
}

void 
FFOsciProcessor::AllocOnDemandBuffers(
  FBRuntimeTopo const* topo, FBProcStateContainer* state, 
  int moduleSlot, bool graph, float sampleRate)
{
  // for graphing we need ALL unison voice delay lines for string since some voice states may not match the main state.
  // just make it easy and allocate it all, we'll release soon and sample rate is low anyway

  auto* procState = state->RawAs<FFProcState>();
  auto const& params = procState->param.voice.osci[moduleSlot];
  auto const& typeNorm = params.block.type[0].GlobalValue();
  auto const& uniCountNorm = params.block.uniCount[0].GlobalValue();
  auto const& moduleTopo = topo->static_->modules[(int)FFModuleType::Osci];

  auto type = moduleTopo.NormalizedToListFast<FFOsciType>(FFOsciParam::Type, typeNorm);
  int uniCount = moduleTopo.NormalizedToDiscreteFast(FFOsciParam::UniCount, uniCountNorm); 
  int oversampleTimes = graph ? 1 : FFOsciOversampleTimes;
  int maxDelayLineSize = static_cast<int>(std::ceil(sampleRate * oversampleTimes / FFOsciStringMinFreq));
  if(graph || type == FFOsciType::String)
    for (int i = 0; i < (graph? FFOsciUniMaxCount: uniCount); i++)
      _stringUniState[i].delayLine.AllocBuffersIfChanged(state->MemoryPool(), maxDelayLineSize);
}

void
FFOsciProcessor::BeginVoice(bool graph, FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];

  auto const& typeNorm = params.block.type[0].Voice()[voice];
  _type = topo.NormalizedToListFast<FFOsciType>(FFOsciParam::Type, typeNorm);
  if (_type == FFOsciType::Off)
    return;

  int modStartSlot = OsciModStartSlot(state.moduleSlot);
  auto const& modParams = procState->param.voice.osciMod[0];
  auto const& modTopo = state.topo->static_->modules[(int)FFModuleType::OsciMod];

  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];
  auto const& uniOffsetNorm = params.block.uniOffset[0].Voice()[voice];
  auto const& uniRandomNorm = params.block.uniRandom[0].Voice()[voice];
  auto const& modExpoFMNorm = modParams.block.expoFM[0].Voice()[voice];
  auto const& modOversampleNorm = modParams.block.oversample[0].Voice()[voice];

  _graph = graph;
  _phaseGen = {};

  auto const& noteEvent = state.voice->event.note;
  _key = static_cast<float>(noteEvent.key);

#if 0 TODO

  // todo note filtering.
  double keyFreqNormal = FBPitchToFreq((float)_key);
  double keyPlus1FreqNormal = FBPitchToFreq((float)_key+1.0f);
  
  double keyFreqRetunedDirect = MTS_NoteToFrequency(procState->mtsClient, (char)noteEvent.key, (char)noteEvent.channel);
  double keyRetunedSemis = MTS_RetuningInSemitones(procState->mtsClient, (char)noteEvent.key, (char)noteEvent.channel);
  double keyFreqRetunedSemisFreq = FBPitchToFreq((float)(_key + keyRetunedSemis));
  double keyRetunedRatio = MTS_RetuningAsRatio(procState->mtsClient, (char)noteEvent.key, (char)noteEvent.channel);
  double keyRetunedSemisByRatio = 12.0 / std::log(2.0) * std::log(keyRetunedRatio);
  double keyFreqRetunedSemisByRatio = FBPitchToFreq((float)(_key + keyRetunedSemisByRatio));
  double keyRetunedPitch = _key + keyRetunedSemis;
  (void)keyRetunedPitch;
  // keyFreqRetunedDirect == keyFreqRetunedSemisFreq == keyFreqRetunedSemisByRatio

  (void)keyRetunedRatio;
  (void)keyRetunedSemisByRatio;
  (void)keyFreqRetunedSemisByRatio;


  double keyPlus1FreqRetunedDirect = MTS_NoteToFrequency(procState->mtsClient, (char)noteEvent.key + 1, (char)noteEvent.channel);
  double keyPlus1RetunedSemis = MTS_RetuningInSemitones(procState->mtsClient, (char)noteEvent.key + 1, (char)noteEvent.channel);
  double keyPlus1FreqRetunedSemisFreq = FBPitchToFreq((float)(_key + 1 + keyPlus1RetunedSemis));
  double keyPlus1RetunedRatio = MTS_RetuningAsRatio(procState->mtsClient, (char)noteEvent.key + 1, (char)noteEvent.channel);
  double keyPlus1RetunedSemisByRatio = 12.0 / std::log(2.0) * std::log(keyPlus1RetunedRatio);
  double keyPlus1FreqRetunedSemisByRatio = FBPitchToFreq((float)(_key + 1 + keyPlus1RetunedSemisByRatio));
  double keyPlus1RetunedPitch = _key + 1 + keyPlus1RetunedSemis;
  (void)keyPlus1RetunedPitch;
  // keyPlus1FreqRetunedDirect == keyPlus1FreqRetunedSemisFreq == keyPlus1FreqRetunedSemisByRatio

  (void)keyPlus1RetunedRatio;
  (void)keyPlus1RetunedSemisByRatio;
  (void)keyPlus1FreqRetunedSemisByRatio;

  double retunedFreqLog[5];
  double retunedPitchLog[5];
  double retunedFreqLinear[5];
  double retunedPitchLinear[5];
  for (int i = 0; i <= 4; i++)
  {
    double keyPlusABit = _key + i / (float)4;
    double keyPlusABitFreqNormal = FBPitchToFreq((float)keyPlusABit);

    int keyLow = (int)keyPlusABit;
    int keyHigh = keyLow + 1;
    double lerp = keyPlusABit - keyLow;

    double keyLowRetunedSemis = MTS_RetuningInSemitones(procState->mtsClient, (char)keyLow, (char)noteEvent.channel);
    double keyHighRetunedSemis = MTS_RetuningInSemitones(procState->mtsClient, (char)keyHigh, (char)noteEvent.channel);
    double keyPlusABitPitchLinear = (1 - lerp) * (keyLow + keyLowRetunedSemis) + lerp * (keyHigh + keyHighRetunedSemis);
    double keyPlusABitFreqLinear = FBPitchToFreq((float)keyPlusABitPitchLinear);
    retunedPitchLinear[i] = keyPlusABitPitchLinear;
    retunedFreqLinear[i] = keyPlusABitFreqLinear;

    double keyLowRetunedRatio = MTS_RetuningAsRatio(procState->mtsClient, (char)keyLow, (char)noteEvent.channel);
    double keyHighRetunedRatio = MTS_RetuningAsRatio(procState->mtsClient, (char)keyHigh, (char)noteEvent.channel);
    double keyPlusABitRetunedRatio = (1 - lerp) * (keyLowRetunedRatio) + lerp * (keyHighRetunedRatio);
    double keyPlusABitRetunedSemisByRatio = 12.0 / std::log(2.0) * std::log(keyPlusABitRetunedRatio);
    double keyPlusABitFreqRetunedSemisByRatio = FBPitchToFreq((float)(keyPlusABit + keyPlusABitRetunedSemisByRatio));
    retunedPitchLog[i] = keyPlusABit + keyPlusABitRetunedSemisByRatio;
    retunedFreqLog[i] = keyPlusABitFreqRetunedSemisByRatio;

    (void)keyPlusABit;
    (void)keyPlusABitFreqNormal;
    (void)keyPlusABitFreqNormal;

    int x = 0;
    x++;
  }

  int y = 0;
  y++;

  (void)keyFreqNormal;
  (void)keyPlus1FreqNormal;

  (void)keyFreqRetunedDirect;
  (void)keyRetunedSemis;
  (void)keyFreqRetunedSemisFreq;

  (void)keyPlus1FreqRetunedDirect;
  (void)keyPlus1RetunedSemis;
  (void)keyPlus1FreqRetunedSemisFreq;

#endif

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
FFOsciProcessor::Process(bool graph, FBModuleProcState& state)
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
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];
  int prevPositionSamplesUpToFirstCycle = _phaseGen.PositionSamplesUpToFirstCycle();

  auto const& panNorm = procParams.acc.pan[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.uniBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.uniDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.uniSpread[0].Voice()[voice];

  auto const& gainNormIn = procParams.acc.gain[0].Voice()[voice];
  auto const& fineNormIn = procParams.acc.fine[0].Voice()[voice];
  auto const& envToGain = procParams.acc.envToGain[0].Voice()[voice];
  auto const& lfoToFine = procParams.acc.lfoToFine[0].Voice()[voice];
  FBSArray<float, FBFixedBlockSamples> gainNormModulated = {};
  FBSArray<float, FBFixedBlockSamples> fineNormModulated = {};

  gainNormIn.CV().CopyTo(gainNormModulated);
  fineNormIn.CV().CopyTo(fineNormModulated);
  if (!graph)
  {
    FFApplyModulation(FFModulationOpType::UPMul, voiceState.env[state.moduleSlot].output, envToGain.CV(), gainNormModulated);
    FFApplyModulation(FFModulationOpType::BPStack, voiceState.vLFO[state.moduleSlot].outputAll, lfoToFine.CV(), fineNormModulated);
  }

  FBSArray<float, FFOsciFixedBlockOversamples> panPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> gainPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> uniBlendPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> uniSpreadPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> uniDetunePlain;
  FBSArray<float, FFOsciFixedBlockOversamples> baseFreqPlain;
  FBSArray<float, FFOsciFixedBlockOversamples> basePitchPlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, s);
    auto fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNormModulated.Load(s));
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    basePitchPlain.Store(s, pitch);
    baseFreqPlain.Store(s, baseFreq);
    _phaseGen.NextBatch(baseFreq / sampleRate);

    panPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::Pan, panNorm, s));
    gainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::Gain, gainNormModulated.Load(s)));
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
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.uniBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.uniDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.uniSpread[0][voice] = uniSpreadNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainNormModulated.Last();
  exchangeParams.acc.fine[0][voice] = fineNormModulated.Last();
  exchangeParams.acc.envToGain[0][voice] = envToGain.Last();
  exchangeParams.acc.lfoToFine[0][voice] = lfoToFine.Last();
  return FBFixedBlockSamples;
}