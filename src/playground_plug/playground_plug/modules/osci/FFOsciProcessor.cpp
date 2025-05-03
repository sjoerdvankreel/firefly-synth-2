#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

// basics from https://github.com/martinfinke/PolyBLEP/blob/master/PolyBLEP.cpp

static inline float constexpr MinPW = 0.05f;

using namespace juce::dsp;

static inline int
OsciModStartSlot(int osciSlot)
{
  switch (osciSlot)
  {
  case 0: return -1;
  case 1: return 0;
  case 2: return 1;
  case 3: return 3;
  default: assert(false); return -1;
  }
}

static inline float
FMRatioRatio(int v)
{
  assert(0 <= v && v < FFOsciFMRatioCount * FFOsciFMRatioCount);
  return ((v / FFOsciFMRatioCount) + 1.0f) / ((v % FFOsciFMRatioCount) + 1.0f);
}

static inline float
GenerateBLEP(float phase, float incr)
{
  if (phase < incr) 
  {
    float b = phase / incr - 1.0f;
    return -(b * b);
  } else if (phase > 1.0f - incr) 
  {
    float b = (phase - 1.0f) / incr + 1.0f;
    return b * b;
  }
  else
    return 0.0f;
}

static inline float
GenerateBLAMP(float phase, float incr)
{
  if (phase < incr) 
  {
    float b = phase / incr - 1.0f;
    return -1.0f / 3.0f * b * b * b;
  } else if (phase > 1.0f - incr) 
  {
    float b = (phase - 1.0f) / incr + 1.0f;
    return 1.0f / 3.0f * b * b * b;
  } else
    return 0.0f;
}

static inline FBSIMDVector<float>
GenerateSaw(
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> yArray;
  FBSIMDArray<float, FBSIMDFloatCount> incrArray;
  FBSIMDArray<float, FBSIMDFloatCount> phaseArray;
  incrArray.Store(0, incrVec);
  phaseArray.Store(0, phaseVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float saw = FBPhaseWrap(phaseArray.Get(i) + 0.5f) * 2.0f - 1.0f;
    float blep = GenerateBLEP(phaseArray.Get(i), incrArray.Get(i));
    yArray.Set(i, saw - blep);
  }
  return yArray.Load(0);
}

static inline FBSIMDVector<float>
GenerateTri(
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec, 
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> yArray;
  FBSIMDArray<float, FBSIMDFloatCount> incrArray;
  FBSIMDArray<float, FBSIMDFloatCount> phaseArray;
  FBSIMDArray<float, FBSIMDFloatCount> realPWArray;
  auto realPWVec = (MinPW + (1.0f - MinPW) * pwVec) * 0.5f;
  incrArray.Store(0, incrVec);
  phaseArray.Store(0, phaseVec);
  realPWArray.Store(0, realPWVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float pw = realPWArray.Get(i);
    float incr = incrArray.Get(i);
    float phase = phaseArray.Get(i);
    float p1 = FBPhaseWrap(phase + 0.5f * pw);
    float p2 = FBPhaseWrap(phase + 1.0f - 0.5f * pw);
    float y = phase * 2.0f;
    if (y >= 2.0f - pw)
      y = (y - 2.0f) / pw;
    else if (y >= pw)
      y = 1.0f - (y - pw) / (1.0f - pw);
    else
      y /= pw;
    y += incr / (pw - pw * pw) * (GenerateBLAMP(p1, incr) - GenerateBLAMP(p2, incr));
    yArray.Set(i, y);
  }
  return yArray.Load(0);
}

static inline FBSIMDVector<float>
GenerateSqr(
  FBSIMDVector<float> phaseVec,
  FBSIMDVector<float> incrVec, 
  FBSIMDVector<float> pwVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> yArray;
  FBSIMDArray<float, FBSIMDFloatCount> incrArray;
  FBSIMDArray<float, FBSIMDFloatCount> phaseArray;
  FBSIMDArray<float, FBSIMDFloatCount> realPWArray;
  auto realPWVec = (MinPW + (1.0f - MinPW) * pwVec) * 0.5f;
  incrArray.Store(0, incrVec);
  phaseArray.Store(0, phaseVec);
  realPWArray.Store(0, realPWVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float pw = realPWArray.Get(i);
    float incr = incrArray.Get(i);
    float phase = phaseArray.Get(i);
    float p2 = FBPhaseWrap(phase + 1.0f - pw);
    float y = -2.0f * pw;
    if (phase < pw)
      y += 2.0f;
    y += GenerateBLEP(phase, incr) - GenerateBLEP(p2, incr);
    yArray.Set(i, y);
  }
  return yArray.Load(0);
}

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
static inline FBSIMDVector<float>
GenerateDSF(
  FBSIMDVector<float> phaseVec, FBSIMDVector<float> freqVec, 
  FBSIMDVector<float> decayVec, FBSIMDVector<float> distFreqVec, 
  FBSIMDVector<float> overtoneVec)
{
  float const decayRange = 0.99f;
  float const scaleFactor = 0.975f;

  auto n = overtoneVec;
  auto w = decayVec * decayRange;
  auto wPowNp1 = xsimd::pow(FBSIMDVector<float>(w), overtoneVec + 1.0f);
  auto u = 2.0f * FBPi * phaseVec;
  auto v = 2.0f * FBPi * distFreqVec * phaseVec / freqVec;
  auto a = w * xsimd::sin(u + n * v) - xsimd::sin(u + (n + 1.0f) * v);
  auto x = (w * xsimd::sin(v - u) + xsimd::sin(u)) + wPowNp1 * a;
  auto y = 1.0f + w * w - 2.0f * w * xsimd::cos(v);
  auto scale = (1.0f - wPowNp1) / (1.0f - w);
  return x * scaleFactor / (y * scale);
}

static inline FBSIMDVector<float>
GenerateDSFOvertones(
  FBSIMDVector<float> phaseVec, FBSIMDVector<float> freqVec, 
  FBSIMDVector<float> decayVec, FBSIMDVector<float> distFreqVec, 
  FBSIMDVector<float> maxOvertoneVec, float overtones_)
{
  auto overtoneVec = xsimd::min(FBSIMDVector<float>(overtones_), xsimd::floor(maxOvertoneVec));
  return GenerateDSF(phaseVec, freqVec, decayVec, distFreqVec, overtoneVec);
}

static inline FBSIMDVector<float>
GenerateDSFBandwidth(
  FBSIMDVector<float> phaseVec, FBSIMDVector<float> freqVec, 
  FBSIMDVector<float> decayVec, FBSIMDVector<float> distFreqVec, 
  FBSIMDVector<float> maxOvertoneVec, float bandwidth)
{
  auto overtoneVec = 1.0f + xsimd::floor(bandwidth * (maxOvertoneVec - 1.0f));
  overtoneVec = xsimd::min(overtoneVec, xsimd::floor(maxOvertoneVec));
  return GenerateDSF(phaseVec, freqVec, decayVec, distFreqVec, overtoneVec);
}

FFOsciProcessor::
FFOsciProcessor() :
_oversampler(
    FFOsciUniMaxCount, FFOsciOversamplingFactor,
    Oversampling<float>::filterHalfBandPolyphaseIIR, false, false)
{
  _oversampler.initProcessing(FBFixedBlockSamples);
  for (int u = 0; u < FFOsciUniMaxCount; u++)
    _downsampledChannelPtrs[u] = _uniOutputDownsampled[u].Ptr(0);
  _downsampledBlock = AudioBlock<float>(_downsampledChannelPtrs.data(), FFOsciUniMaxCount, 0, FBFixedBlockSamples);
  _oversampledBlock = _oversampler.processSamplesUp(_downsampledBlock);
}

void
FFOsciProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  int modStartSlot = OsciModStartSlot(state.moduleSlot);
  auto const& modParams = procState->param.voice.osciMod[0];
  auto const& modTopo = state.topo->static_.modules[(int)FFModuleType::OsciMod];

  auto const& typeNorm = params.block.type[0].Voice()[voice];
  auto const& uniCountNorm = params.block.uniCount[0].Voice()[voice];
  auto const& uniOffsetNorm = params.block.uniOffset[0].Voice()[voice];
  auto const& uniRandomNorm = params.block.uniRandom[0].Voice()[voice];
  auto const& basicSinOnNorm = params.block.basicSinOn[0].Voice()[voice];
  auto const& basicSawOnNorm = params.block.basicSawOn[0].Voice()[voice];
  auto const& basicTriOnNorm = params.block.basicTriOn[0].Voice()[voice];
  auto const& basicSqrOnNorm = params.block.basicSqrOn[0].Voice()[voice];
  auto const& dsfModeNorm = params.block.dsfMode[0].Voice()[voice];
  auto const& dsfDistanceNorm = params.block.dsfDistance[0].Voice()[voice];
  auto const& dsfOvertonesNorm = params.block.dsfOvertones[0].Voice()[voice];
  auto const& dsfBandwidthNorm = params.block.dsfBandwidth[0].Voice()[voice];
  auto const& fmExpNorm = params.block.fmExp[0].Voice()[voice];
  auto const& fmRatioModeNorm = params.block.fmRatioMode[0].Voice()[voice];
  auto const& fmRatioRatio12Norm = params.block.fmRatioRatio[0].Voice()[voice];
  auto const& fmRatioRatio23Norm = params.block.fmRatioRatio[1].Voice()[voice];

  auto const& modExpoFMNorm = modParams.block.expoFM[0].Voice()[voice];
  auto const& modOversamplingNorm = modParams.block.oversampling[0].Voice()[voice];

  _phaseGen = {};
  _prng = FBParkMillerPRNG(state.moduleSlot / static_cast<float>(FFOsciCount));

  _key = static_cast<float>(state.voice->event.note.key);
  _type = topo.NormalizedToListFast<FFOsciType>(FFOsciParam::Type, typeNorm);
  _uniCount = topo.NormalizedToDiscreteFast(FFOsciParam::UniCount, uniCountNorm);
  _uniOffsetPlain = topo.NormalizedToIdentityFast(FFOsciParam::UniOffset, uniOffsetNorm);
  _uniRandomPlain = topo.NormalizedToIdentityFast(FFOsciParam::UniRandom, uniRandomNorm);
  _basicSinOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSinOn, basicSinOnNorm);
  _basicSawOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSawOn, basicSawOnNorm);
  _basicTriOn = topo.NormalizedToBoolFast(FFOsciParam::BasicTriOn, basicTriOnNorm);
  _basicSqrOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSqrOn, basicSqrOnNorm);
  _dsfMode = topo.NormalizedToListFast<FFOsciDSFMode>(FFOsciParam::DSFMode, dsfModeNorm);
  _dsfBandwidthPlain = topo.NormalizedToLog2Fast(FFOsciParam::DSFBandwidth, dsfBandwidthNorm);
  _dsfDistance = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::DSFDistance, dsfDistanceNorm));
  _dsfOvertones = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::DSFOvertones, dsfOvertonesNorm));
  _fmExp = topo.NormalizedToBoolFast(FFOsciParam::FMExp, fmExpNorm);
  _fmRatioMode = topo.NormalizedToListFast<FFOsciFMRatioMode>(FFOsciParam::FMRatioMode, fmRatioModeNorm);
  _fmRatioRatio12 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, fmRatioRatio12Norm));
  _fmRatioRatio23 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, fmRatioRatio23Norm));

  bool oversampling = modTopo.NormalizedToBoolFast(FFOsciModParam::Oversampling, modOversamplingNorm);
  _modMatrixExpoFM = modTopo.NormalizedToBoolFast(FFOsciModParam::ExpoFM, modExpoFMNorm);
  _oversamplingTimes = oversampling ? FFOsciOversamplingTimes : 1;

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
  
  FBSIMDArray<float, FFOsciUniMaxCount> uniPhaseInit = {};
  for (int u = 0; u < _uniCount; u++)
  {
    float random = _uniRandomPlain;
    float uniPhase = u * _uniOffsetPlain / _uniCount;
    uniPhaseInit.Set(u, ((1.0f - _uniRandomPlain) + _uniRandomPlain * _prng.Next()) * uniPhase);
    _uniPhaseGens[u] = FFOsciPhaseGenerator(uniPhaseInit.Get(u));
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
  }

  if (_type == FFOsciType::FM)
  {
    _prevUniFMOutput.Fill(0.0f);
    for (int o = 0; o < FFOsciFMOperatorCount; o++)
      for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      {
        auto phaseInits = FBSIMDVector<float>::load_aligned(uniPhaseInit.Ptr(0) + u);
        _uniFMPhaseGens[o][u / FBSIMDFloatCount] = FFOsciFMPhaseGenerator(phaseInits);
      }
  }
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
  float oversampledRate = sampleRate * _oversamplingTimes;
  int totalSamples = FBFixedBlockSamples * _oversamplingTimes;
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  int prevPositionSamplesUpToFirstCycle = _phaseGen.PositionSamplesUpToFirstCycle();

  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& gLFOToGainNorm = procParams.acc.gLFOToGain[0].Voice()[voice];
  auto const& uniBlendNorm = procParams.acc.unisonBlend[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.unisonDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.unisonSpread[0].Voice()[voice];
  auto const& dsfDecayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  auto const& basicPWNorm = procParams.acc.basicPW[0].Voice()[voice];
  auto const& basicSqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];
  auto const& basicSinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];  

  FBSIMDArray<float, FFOsciFixedBlockOversamples> gainPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> baseFreqPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basePitchPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> uniBlendPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> uniDetunePlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> uniSpreadPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basicPWPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basicSqrGainPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basicSinGainPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basicSawGainPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basicTriGainPlain;
  FBSIMDArray<float, FFOsciFixedBlockOversamples> dsfDecayPlain;
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciFMMatrixSize> fmIndexPlain;
  FBSIMDArray2<float, FFOsciFixedBlockOversamples, FFOsciFMOperatorCount - 1> fmRatioPlain;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto fine = topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm, s);
    auto coarse = topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, s);
    auto pitch = _key + coarse + fine;
    auto baseFreq = FBPitchToFreq(pitch);
    basePitchPlain.Store(s, pitch);
    baseFreqPlain.Store(s, baseFreq);
    _phaseGen.Next(baseFreq / sampleRate);

    gainPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::Gain, gainNorm, s));
    uniBlendPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniBlend, uniBlendNorm, s));
    uniDetunePlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniDetune, uniDetuneNorm, s));
    uniSpreadPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::UniSpread, uniSpreadNorm, s));

    if (_type == FFOsciType::Basic)
    {
      if (_basicSqrOn || _basicTriOn)
        basicPWPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::BasicPW, basicPWNorm, s));
      if (_basicSinOn)
        basicSinGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicSinGain, basicSinGainNorm, s));
      if (_basicSawOn)
        basicSawGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicSawGain, basicSawGainNorm, s));
      if (_basicTriOn)
        basicTriGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicTriGain, basicTriGainNorm, s));
      if (_basicSqrOn)
        basicSqrGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicSqrGain, basicSqrGainNorm, s));
    }
    else if (_type == FFOsciType::DSF)
      dsfDecayPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::DSFDecay, dsfDecayNorm, s));
    else if (_type == FFOsciType::FM)
    {
      for (int m = 0; m < FFOsciFMMatrixSize; m++)
      {
        auto const& fmIndexNorm = procParams.acc.fmIndex[m].Voice()[voice];
        fmIndexPlain[m].Store(s, topo.NormalizedToLog2Fast(FFOsciParam::FMIndex, fmIndexNorm, s));
      }
      if (_fmRatioMode == FFOsciFMRatioMode::Ratio)
      {
        fmRatioPlain[0].Store(s, _fmRatioRatio12);
        fmRatioPlain[1].Store(s, _fmRatioRatio23);
      }
      else
        for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
        {
          auto const& fmRatioFreeNorm = procParams.acc.fmRatioFree[o].Voice()[voice];
          fmRatioPlain[o].Store(s, topo.NormalizedToLog2Fast(FFOsciParam::FMRatioFree, fmRatioFreeNorm, s));
        }
    }
    else assert(false);
  }
  if (_oversamplingTimes != 1)
  {
    gainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    basePitchPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    baseFreqPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    uniBlendPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    uniDetunePlain.UpsampleStretch<FFOsciOversamplingTimes>();
    uniSpreadPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    if (_type == FFOsciType::Basic)
    {
      if(_basicSqrOn || _basicTriOn)
        basicPWPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      if (_basicSinOn)
        basicSinGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      if (_basicSawOn)
        basicSawGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      if (_basicTriOn)
        basicTriGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      if (_basicSqrOn)
        basicSqrGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    }
    else if (_type == FFOsciType::DSF)
      dsfDecayPlain.UpsampleStretch<FFOsciOversamplingTimes>();
    else if (_type == FFOsciType::FM)
    {
      for (int m = 0; m < FFOsciFMMatrixSize; m++)
        fmIndexPlain[m].UpsampleStretch<FFOsciOversamplingTimes>();
      for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
        fmRatioPlain[o].UpsampleStretch<FFOsciOversamplingTimes>();
    } else assert(false);
  }

  float applyModMatrixExpoFM = _modMatrixExpoFM ? 1.0f : 0.0f;
  float applyModMatrixLinearFM = _modMatrixExpoFM ? 0.0f : 1.0f;
  if (_type != FFOsciType::FM)
  {
    for (int u = 0; u < _uniCount; u++)
    {
      float uniPosMHalfToHalf = _uniPosMHalfToHalf.Get(u);
      for (int s = 0; s < totalSamples; s += FBSIMDFloatCount)
      {
        FBSIMDVector<float> matrixFMMod(0.0f);
        for (int src = 0; src < state.moduleSlot; src++)
          if (_modSourceFMOn[src] && _modSourceUniCount[src] > u)
          {
            int modSlot = OsciModStartSlot(state.moduleSlot) + src;
            auto const& thatUniOutput = voiceState.osci[src].uniOutputOversampled[u].Load(s);
            matrixFMMod += thatUniOutput * voiceState.osciMod.outputFMIndex[modSlot].Load(s);
          }

        auto uniPitch = basePitchPlain.Load(s);
        uniPitch += uniPosMHalfToHalf * uniDetunePlain.Load(s);
        uniPitch += matrixFMMod * uniPitch * applyModMatrixExpoFM;

        auto uniFreq = FBPitchToFreq(uniPitch);
        auto uniIncr = uniFreq / oversampledRate;
        auto uniPhase = _uniPhaseGens[u].Next(uniIncr, matrixFMMod * applyModMatrixLinearFM);

        FBSIMDVector<float> thisUniOutput = 0.0f;
        if (_type == FFOsciType::Basic)
        {
          if (_basicSawOn)
          {
            auto basicSawGain = basicSawGainPlain.Load(s);
            thisUniOutput += GenerateSaw(uniPhase, uniIncr) * basicSawGain;
          }
          if (_basicSinOn)
          {
            auto basicSinGain = basicSinGainPlain.Load(s);
            thisUniOutput += xsimd::sin(uniPhase * FBTwoPi) * basicSinGain;
          }
          if (_basicTriOn)
          {
            auto basicPW = basicPWPlain.Load(s);
            auto basicTriGain = basicTriGainPlain.Load(s);
            thisUniOutput += GenerateTri(uniPhase, uniIncr, basicPW) * basicTriGain;
          }
          if (_basicSqrOn)
          {
            auto basicPW = basicPWPlain.Load(s);
            auto basicSqrGain = basicSqrGainPlain.Load(s);
            thisUniOutput += GenerateSqr(uniPhase, uniIncr, basicPW) * basicSqrGain;
          }
        }
        else if (_type == FFOsciType::DSF)
        {
          auto dsfDecay = dsfDecayPlain.Load(s);
          auto dsfDistFreq = _dsfDistance * uniFreq;
          auto dsfMaxOvertones = (sampleRate * 0.5f - uniFreq) / dsfDistFreq;
          if (_dsfMode == FFOsciDSFMode::Overtones)
            thisUniOutput += GenerateDSFOvertones(uniPhase, uniFreq, dsfDecay, dsfDistFreq, dsfMaxOvertones, _dsfOvertones);
          else if (_dsfMode == FFOsciDSFMode::Bandwidth)
            thisUniOutput += GenerateDSFBandwidth(uniPhase, uniFreq, dsfDecay, dsfDistFreq, dsfMaxOvertones, _dsfBandwidthPlain);
          else assert(false);
        }
        else assert(false);
        uniOutputOversampled[u].Store(s, thisUniOutput);
      }
    }
  }

  if (_type == FFOsciType::FM)
  {
    for (int s = 0; s < totalSamples; s++)
      for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      {
        int block = u / FBSIMDFloatCount;
        auto uniPosMHalfToHalf = _uniPosMHalfToHalf.Load(u);

        FBSIMDVector<float> matrixFMMod(0.0f);
        for (int src = 0; src < state.moduleSlot; src++)
          if (_modSourceFMOn[src] && _modSourceUniCount[src] > u)
          {
            int modSlot = OsciModStartSlot(state.moduleSlot) + src;
            float thatUniOutput = voiceState.osci[src].uniOutputOversampled[u].Get(s);
            matrixFMMod += thatUniOutput * voiceState.osciMod.outputFMIndex[modSlot].Get(s);
          }

        FBSIMDVector<float> op3UniPitch = basePitchPlain.Get(s) + uniPosMHalfToHalf * uniDetunePlain.Get(s);
        op3UniPitch += matrixFMMod * op3UniPitch * applyModMatrixExpoFM;
        auto op3UniFreq = FBPitchToFreq(op3UniPitch);

        FBSIMDVector<float> op1UniPhase;
        FBSIMDVector<float> fmTo1 = 0.0f;
        fmTo1 += fmIndexPlain[0].Get(s) * _prevUniFMOutput[0].Load(u);
        fmTo1 += fmIndexPlain[3].Get(s) * _prevUniFMOutput[1].Load(u);
        fmTo1 += fmIndexPlain[6].Get(s) * _prevUniFMOutput[2].Load(u);
        if (_fmExp)
        {
          auto op1UniPitch = op3UniPitch / fmRatioPlain[1].Get(s) / fmRatioPlain[0].Get(s);
          op1UniPitch += op1UniPitch * fmTo1;
          auto op1UniFreq = FBPitchToFreq(op1UniPitch);
          op1UniPhase = _uniFMPhaseGens[0][block].Next(op1UniFreq / oversampledRate, 0.0f);
        }
        else
        {
          auto op1UniFreq = op3UniFreq / fmRatioPlain[1].Get(s) / fmRatioPlain[0].Get(s);
          op1UniPhase = _uniFMPhaseGens[0][block].Next(op1UniFreq / oversampledRate, fmTo1);
        }
        auto output1 = xsimd::sin(op1UniPhase * FBTwoPi);

        FBSIMDVector<float> op2UniPhase;
        FBSIMDVector<float> fmTo2 = 0.0f;
        fmTo2 += fmIndexPlain[1].Get(s) * output1;
        fmTo2 += fmIndexPlain[4].Get(s) * _prevUniFMOutput[1].Load(u);
        fmTo2 += fmIndexPlain[7].Get(s) * _prevUniFMOutput[2].Load(u);
        if (_fmExp)
        {
          auto op2UniPitch = op3UniPitch / fmRatioPlain[1].Get(s);
          op2UniPitch += op2UniPitch * fmTo2;
          auto op2UniFreq = FBPitchToFreq(op2UniPitch);
          op2UniPhase = _uniFMPhaseGens[1][block].Next(op2UniFreq / oversampledRate, 0.0f);
        }
        else
        {
          auto op2UniFreq = op3UniFreq / fmRatioPlain[1].Get(s);
          op2UniPhase = _uniFMPhaseGens[1][block].Next(op2UniFreq / oversampledRate, fmTo2);
        }
        auto output2 = xsimd::sin(op2UniPhase * FBTwoPi);

        FBSIMDVector<float> op3UniPhase;
        FBSIMDVector<float> fmTo3 = 0.0f;
        fmTo3 += fmIndexPlain[2].Get(s) * output1;
        fmTo3 += fmIndexPlain[5].Get(s) * output2;
        fmTo3 += fmIndexPlain[8].Get(s) * _prevUniFMOutput[2].Load(u);
        if (_fmExp)
        {
          op3UniPitch += op3UniPitch * fmTo3;
          auto op3UniFreq = FBPitchToFreq(op3UniPitch);
          op3UniPhase = _uniFMPhaseGens[2][block].Next(op3UniFreq / oversampledRate, 0.0f);
        }
        else
        {
          op3UniPhase = _uniFMPhaseGens[2][block].Next(op3UniFreq / oversampledRate, fmTo3 + matrixFMMod);
        }
        auto output3 = xsimd::sin(op3UniPhase * FBTwoPi);

        _prevUniFMOutput[0].Store(u, output1);
        _prevUniFMOutput[1].Store(u, output2);
        _prevUniFMOutput[2].Store(u, output3);

        FBSIMDArray<float, FBSIMDFloatCount> outputArray;
        outputArray.Store(0, output3);
        for (int v = 0; v < FBSIMDFloatCount; v++)
          uniOutputOversampled[u + v].Set(s, outputArray.Get(v));
      }
  }

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

  if (_oversamplingTimes == 1)
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        _uniOutputDownsampled[u].Store(s, uniOutputOversampled[u].Load(s));
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
      auto uniPanning = 0.5f + uniPosMHalfToHalf * uniSpreadPlain.Load(s);
      auto uniBlend = 1.0f - (uniPosAbsHalfToHalf * 2.0f * (1.0f - uniBlendPlain.Load(s)));
      auto uniMono = _uniOutputDownsampled[u].Load(s) * gainPlain.Load(s) * uniBlend;
      output[0].Add(s, (1.0f - uniPanning) * uniMono);
      output[1].Add(s, uniPanning * uniMono);
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phaseGen.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreqPlain.Get(_oversamplingTimes * FBFixedBlockSamples - 1), state.input->sampleRate);
  exchangeDSP.positionSamples = _phaseGen.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.gain[0][voice] = gainNorm.Last();
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGainNorm.Last();
  exchangeParams.acc.unisonBlend[0][voice] = uniBlendNorm.Last();
  exchangeParams.acc.unisonDetune[0][voice] = uniDetuneNorm.Last();
  exchangeParams.acc.unisonSpread[0][voice] = uniSpreadNorm.Last();
  exchangeParams.acc.dsfDecay[0][voice] = dsfDecayNorm.Last();
  exchangeParams.acc.basicPW[0][voice] = basicPWNorm.Last();
  exchangeParams.acc.basicSinGain[0][voice] = basicSinGainNorm.Last();
  exchangeParams.acc.basicSawGain[0][voice] = basicSawGainNorm.Last();
  exchangeParams.acc.basicTriGain[0][voice] = basicTriGainNorm.Last();
  exchangeParams.acc.basicSqrGain[0][voice] = basicSqrGainNorm.Last();
  for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
  {
    auto const& fmRatioFreeNorm = procParams.acc.fmRatioFree[o].Voice()[voice];
    exchangeParams.acc.fmRatioFree[o][voice] = fmRatioFreeNorm.Last();
  }
  for (int m = 0; m < FFOsciFMMatrixSize; m++)
  {
    auto const& fmIndexNorm = procParams.acc.fmIndex[m].Voice()[voice];
    exchangeParams.acc.fmIndex[m][voice] = fmIndexNorm.Last();
  }
  return FBFixedBlockSamples;
}

#if 0

/*
PolyBLEP Waveform generator ported from the Jesusonic code by Tale
http://www.taletn.com/reaper/mono_synth/

Permission has been granted to release this port under the WDL/IPlug license:

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#include "PolyBLEP.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <cmath>
#include <cstdint>

const double TWO_PI = 2 * M_PI;

template<typename T>
inline T square_number(const T& x) {
  return x * x;
}

// Adapted from "Phaseshaping Oscillator Algorithms for Musical Sound
// Synthesis" by Jari Kleimola, Victor Lazzarini, Joseph Timoney, and Vesa
// Valimaki.
// http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/
inline double blep(double t, double dt) {
  if (t < dt) {
    return -square_number(t / dt - 1);
  }
  else if (t > 1 - dt) {
    return square_number((t - 1) / dt + 1);
  }
  else {
    return 0;
  }
}

// Derived from blep().
inline double blamp(double t, double dt) {
  if (t < dt) {
    t = t / dt - 1;
    return -1 / 3.0 * square_number(t) * t;
  }
  else if (t > 1 - dt) {
    t = (t - 1) / dt + 1;
    return 1 / 3.0 * square_number(t) * t;
  }
  else {
    return 0;
  }
}

template<typename T>
inline int64_t bitwiseOrZero(const T& t) {
  return static_cast<int64_t>(t) | 0;
}

PolyBLEP::PolyBLEP(double sampleRate, Waveform waveform, double initialFrequency)
  : waveform(waveform), sampleRate(sampleRate), amplitude(1.0), t(0.0) {
  setSampleRate(sampleRate);
  setFrequency(initialFrequency);
  setWaveform(waveform);
  setPulseWidth(0.5);
}

PolyBLEP::~PolyBLEP() {

}

void PolyBLEP::setdt(double time) {
  freqInSecondsPerSample = time;
}

void PolyBLEP::setFrequency(double freqInHz) {
  setdt(freqInHz / sampleRate);
}

void PolyBLEP::setSampleRate(double sampleRate) {
  const double freqInHz = getFreqInHz();
  this->sampleRate = sampleRate;
  setFrequency(freqInHz);
}

double PolyBLEP::getFreqInHz() const {
  return freqInSecondsPerSample * sampleRate;
}

void PolyBLEP::setPulseWidth(double pulseWidth) {
  this->pulseWidth = pulseWidth;
}

void PolyBLEP::sync(double phase) {
  t = phase;
  if (t >= 0) {
    t -= bitwiseOrZero(t);
  }
  else {
    t += 1 - bitwiseOrZero(t);
  }
}

void PolyBLEP::setWaveform(Waveform waveform) {
  this->waveform = waveform;
}

double PolyBLEP::get() const {
  if (getFreqInHz() >= sampleRate / 4) {
    return sin();
  }
  else switch (waveform) {
  case SINE:
    return sin();
  case COSINE:
    return cos();
  case TRIANGLE:
    return tri();
  case SQUARE:
    return sqr();
  case RECTANGLE:
    return rect();
  case SAWTOOTH:
    return saw();
  case RAMP:
    return ramp();
  case MODIFIED_TRIANGLE:
    return tri2();
  case MODIFIED_SQUARE:
    return sqr2();
  case HALF_WAVE_RECTIFIED_SINE:
    return half();
  case FULL_WAVE_RECTIFIED_SINE:
    return full();
  case TRIANGULAR_PULSE:
    return trip();
  case TRAPEZOID_FIXED:
    return trap();
  case TRAPEZOID_VARIABLE:
    return trap2();
  default:
    return 0.0;
  }
}

double PolyBLEP::half() const {
  double t2 = t + 0.5;
  t2 -= bitwiseOrZero(t2);

  double y = (t < 0.5 ? 2 * std::sin(TWO_PI * t) - 2 / M_PI : -2 / M_PI);
  y += TWO_PI * freqInSecondsPerSample * (blamp(t, freqInSecondsPerSample) + blamp(t2, freqInSecondsPerSample));

  return amplitude * y;
}

double PolyBLEP::full() const {
  double _t = this->t + 0.25;
  _t -= bitwiseOrZero(_t);

  double y = 2 * std::sin(M_PI * _t) - 4 / M_PI;
  y += TWO_PI * freqInSecondsPerSample * blamp(_t, freqInSecondsPerSample);

  return amplitude * y;
}

double PolyBLEP::trip() const {
  double t1 = t + 0.75 + 0.5 * pulseWidth;
  t1 -= bitwiseOrZero(t1);

  double y;
  if (t1 >= pulseWidth) {
    y = -pulseWidth;
  }
  else {
    y = 4 * t1;
    y = (y >= 2 * pulseWidth ? 4 - y / pulseWidth - pulseWidth : y / pulseWidth - pulseWidth);
  }

  if (pulseWidth > 0) {
    double t2 = t1 + 1 - 0.5 * pulseWidth;
    t2 -= bitwiseOrZero(t2);

    double t3 = t1 + 1 - pulseWidth;
    t3 -= bitwiseOrZero(t3);
    y += 2 * freqInSecondsPerSample / pulseWidth * (blamp(t1, freqInSecondsPerSample) - 2 * blamp(t2, freqInSecondsPerSample) + blamp(t3, freqInSecondsPerSample));
  }
  return amplitude * y;
}

double PolyBLEP::trap2() const {
  double pulseWidth = std::fmin(0.9999, this->pulseWidth);
  double scale = 1 / (1 - pulseWidth);

  double y = 4 * t;
  if (y >= 3) {
    y -= 4;
  }
  else if (y > 1) {
    y = 2 - y;
  }
  y = std::fmax(-1, std::fmin(1, scale * y));

  double t1 = t + 0.25 - 0.25 * pulseWidth;
  t1 -= bitwiseOrZero(t1);

  double t2 = t1 + 0.5;
  t2 -= bitwiseOrZero(t2);

  // Triangle #1
  y += scale * 2 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

  t1 = t + 0.25 + 0.25 * pulseWidth;
  t1 -= bitwiseOrZero(t1);

  t2 = t1 + 0.5;
  t2 -= bitwiseOrZero(t2);

  // Triangle #2
  y += scale * 2 * freqInSecondsPerSample * (blamp(t1, freqInSecondsPerSample) - blamp(t2, freqInSecondsPerSample));

  return amplitude * y;
}

#endif