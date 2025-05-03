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

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
static inline float
GenerateBLEP(float phase, float incr)
{
  float blep = 0.0f;
  if (phase < incr)
  {
    blep = phase / incr;
    blep = (2.0f - blep) * blep - 1.0f;
  }
  else if (phase >= 1.0f - incr)
  {
    blep = (phase - 1.0f) / incr;
    blep = (blep + 2.0f) * blep + 1.0f;
  }
  return blep;
}

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
static inline float
GenerateBLAMP(float phase, float incr)
{
  float y = 0.0f;
  if (!(0.0f <= phase && phase < 2.0f * incr))
    return y * incr / 15;
  float x = phase / incr;
  float u = 2.0f - x;
  u *= u * u * u * u;
  y -= u;
  if (phase >= incr)
    return y * incr / 15;
  float v = 1.0f - x;
  v *= v * v * v * v;
  y += 4 * v;
  return y * incr / 15;
}

static inline FBSIMDVector<float>
GenerateSaw(FBSIMDVector<float> phaseVec, FBSIMDVector<float> incrVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> yArray;
  FBSIMDArray<float, FBSIMDFloatCount> incrArray;
  FBSIMDArray<float, FBSIMDFloatCount> phaseArray;
  incrArray.Store(0, incrVec);
  phaseArray.Store(0, phaseVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float saw = phaseArray.Get(i) * 2.0f - 1.0f;
    float blep = GenerateBLEP(phaseArray.Get(i), incrArray.Get(i));
    yArray.Set(i, saw - blep);
  }
  return yArray.Load(0);
}

static inline FBSIMDVector<float>
GenerateSqr(
  FBSIMDVector<float> sawVec, FBSIMDVector<float> phaseVec, 
  FBSIMDVector<float> incrVec, FBSIMDVector<float> pwVec)
{
  FBSIMDVector<float> minPW = 0.05f;
  auto realPW = (minPW + (1.0f - minPW) * pwVec) * 0.5f;
  auto phase2 = phaseVec + realPW;
  phase2 -= xsimd::floor(phase2);
  return (sawVec - GenerateSaw(phase2, incrVec)) * 0.5f;
}

static inline FBSIMDVector<float>
GenerateTri(FBSIMDVector<float> phaseVec, FBSIMDVector<float> incrVec)
{
  FBSIMDArray<float, FBSIMDFloatCount> yArray;
  FBSIMDArray<float, FBSIMDFloatCount> incrArray;
  FBSIMDArray<float, FBSIMDFloatCount> phaseArray;
  incrArray.Store(0, incrVec);
  phaseArray.Store(0, phaseVec);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float incr = incrArray.Get(i);
    float phase = phaseArray.Get(i);
    float y = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
    y += GenerateBLAMP(phase, incr);
    y += GenerateBLAMP(1.0f - phase, incr);
    phase += 0.5f;
    phase = FBPhaseWrap(phase);
    y -= GenerateBLAMP(phase, incr);
    y -= GenerateBLAMP(1.0f - phase, incr);
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

#if 0 // todo

// calculate 4 outputs in unison dimension for the fm generator
template <bool ExpoFM>
xsimd::batch<float, FBXSIMDBatchType>
FFOsciProcessor::CalcOneSampleForFM(
  float oversampledRate, int subUniBlock, int op,
  float* uniPitchesForFM, float* uniIncrsForFM,
  xsimd::batch<float, FBXSIMDBatchType> fmToOp,
  xsimd::batch<float, FBXSIMDBatchType> externalFMModulatorsForFMBatch)
{
#if 0
  if constexpr (ExpoFM)
  {
    auto uniPitchOpBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(uniPitchesForFM);
    uniPitchOpBatch += fmToOp * uniPitchOpBatch;
    auto uniPitchOpFreq = 440.0f * xsimd::pow(xsimd::batch<float, FBXSIMDBatchType>(2.0f), (uniPitchOpBatch - 69.0f) / 12.0f);
    auto uniIncrOpBatch = uniPitchOpFreq / oversampledRate;
    return xsimd::sin(_uniPhaseGensForFM[op][subUniBlock].Next(uniIncrOpBatch, externalFMModulatorsForFMBatch) * FBTwoPi);
  }
  else
  {
    auto uniIncrOpBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(uniIncrsForFM);
    return xsimd::sin(_uniPhaseGensForFM[op][subUniBlock].Next(uniIncrOpBatch, fmToOp + externalFMModulatorsForFMBatch) * FBTwoPi);
  }
#endif
}

// vectorization in the unison dimension,
// phases have data dependency in the time domain because feedback loops
template <bool ExpoFM>
void
FFOsciProcessor::ProcessFM(
  FBModuleProcState& state,
  int oversamplingTimes,
  float oversampledRate)
{
#if 0
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& unisonOutputMaybeOversampled = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutputMaybeOversampled;

  std::array<FBFixedFloatArray, FFOsciFMMatrixSize> fmIndexPlain;
  std::array<FBFixedFloatArray, FFOsciFMOperatorCount - 1> fmRatioPlain;
  for (int m = 0; m < FFOsciFMMatrixSize; m++)
  {
    auto const& fmIndexNorm = procParams.acc.fmIndex[m].Voice()[voice];
    topo.NormalizedToLog2Fast(FFOsciParam::FMIndex, fmIndexNorm, fmIndexPlain[m]);
  }
  if (_voiceState.fmRatioMode == FFOsciFMRatioMode::Ratio)
  {
    fmRatioPlain[0].Fill(_voiceState.fmRatioRatio12);
    fmRatioPlain[1].Fill(_voiceState.fmRatioRatio23);
  }
  else
    for (int o = 0; o < FFOsciFMOperatorCount - 1; o++)
    {
      auto const& fmRatioFreeNorm = procParams.acc.fmRatioFree[o].Voice()[voice];
      topo.NormalizedToLog2Fast(FFOsciParam::FMRatioFree, fmRatioFreeNorm, fmRatioPlain[o]);
    }

  // these are the external mods from the inter-osci matrix
  // we only need to apply them here if the mod matrix is doing linear fm
  // in the case of expo fm they have already been applied to the pitch
  // and hence to the incoming freq and delta parameters
  float applyExternalLinearFM = _voiceState.externalFMExp ? 0.0f : 1.0f;
  for (int os = 0; os < oversamplingTimes; os++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      for (int u = 0; u < FFOsciUnisonMaxCount; u++)
        _externalFMModulatorsForFM[os][s][u] = _modMatrixFMModulators[u][os][s] * applyExternalLinearFM;

  // calculate op1/2/3 delta according to 1:2 and 2:3 ratio for expo case (ratio applies to pitch)
  if constexpr (ExpoFM)
  {
    for (int o = 0; o < FFOsciFMOperatorCount; o++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int u = 0; u < FFOsciUnisonMaxCount; u++)
            _uniPitchesForFM[o][os][s][u] = 0.0f;
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
        {
          float op3Pitch = _uniPitches[u][os][s];
          float op2Pitch = op3Pitch / fmRatioPlain[1][s];
          float op1Pitch = op2Pitch / fmRatioPlain[0][s];
          _uniPitchesForFM[0][os][s][u] = op1Pitch;
          _uniPitchesForFM[1][os][s][u] = op2Pitch;
          _uniPitchesForFM[2][os][s][u] = op3Pitch;
        }
  }

  // calculate op1/2/3 delta according to 1:2 and 2:3 ratio for linear case
  // for expo case, we have to do it inside the per-sample loop because ops modulate pitch
  if constexpr (!ExpoFM)
  {
    for (int o = 0; o < FFOsciFMOperatorCount; o++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int u = 0; u < FFOsciUnisonMaxCount; u++)
            _uniIncrsForFM[o][os][s][u] = 0.0f;
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
        {
          float op3Freq = _uniFreqs[u][os][s];
          float op2Freq = op3Freq / fmRatioPlain[1][s];
          float op1Freq = op2Freq / fmRatioPlain[0][s];
          _uniIncrsForFM[0][os][s][u] = op1Freq / oversampledRate;
          _uniIncrsForFM[1][os][s][u] = op2Freq / oversampledRate;
          _uniIncrsForFM[2][os][s][u] = op3Freq / oversampledRate;
        }
  }

  // calculate 3op fm with 3x3 matrix and vectorize over unison
  // for every feedback path there is unit delay
  int oversampledIndex = 0;
  for (int os = 0; os < oversamplingTimes; os++)
    for (int s = 0; s < FBFixedBlockSamples; s++, oversampledIndex++)
    {
      int nonOversampledIndex = oversampledIndex / oversamplingTimes;
      for (int u = 0; u < _voiceState.unisonCount; u += FBSIMDFloatCount)
      {
        int subUniBlock = u / FBSIMDFloatCount;

        xsimd::batch<float, FBXSIMDBatchType> fmTo1 = 0.0f;
        fmTo1 += fmIndexPlain[0][nonOversampledIndex] * _prevUniOutputForFM[0][subUniBlock];
        fmTo1 += fmIndexPlain[3][nonOversampledIndex] * _prevUniOutputForFM[1][subUniBlock];
        fmTo1 += fmIndexPlain[6][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto output1 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 0,
          _uniPitchesForFM[0][os][s].data() + u,
          _uniIncrsForFM[0][os][s].data() + u, fmTo1,
          0.0f);

        xsimd::batch<float, FBXSIMDBatchType> fmTo2 = 0.0f;
        fmTo2 += fmIndexPlain[1][nonOversampledIndex] * output1;
        fmTo2 += fmIndexPlain[4][nonOversampledIndex] * _prevUniOutputForFM[1][subUniBlock];
        fmTo2 += fmIndexPlain[7][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto output2 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 1,
          _uniPitchesForFM[1][os][s].data() + u,
          _uniIncrsForFM[1][os][s].data() + u, fmTo2,
          0.0f);

        // op3 is output, it also takes the external inter-osci fm
        xsimd::batch<float, FBXSIMDBatchType> fmTo3 = 0.0f;
        fmTo3 += fmIndexPlain[2][nonOversampledIndex] * output1;
        fmTo3 += fmIndexPlain[5][nonOversampledIndex] * output2;
        fmTo3 += fmIndexPlain[8][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto externalFMModulatorsForFMBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(_externalFMModulatorsForFM[os][s].data() + u);
        auto output3 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 2,
          _uniPitchesForFM[2][os][s].data() + u,
          _uniIncrsForFM[2][os][s].data() + u, fmTo3,
          externalFMModulatorsForFMBatch);

        _prevUniOutputForFM[0][subUniBlock] = output1;
        _prevUniOutputForFM[1][subUniBlock] = output2;
        _prevUniOutputForFM[2][subUniBlock] = output3;

        alignas(FBSIMDAlign) std::array<float, FBSIMDFloatCount> outputArray;
        output3.store_aligned(outputArray.data());
        for (int v = 0; v < FBSIMDFloatCount; v++)
          unisonOutputMaybeOversampled[u + v][os][s] = outputArray[v];
      }
    }
#endif
}

#endif

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
  auto const& basicSqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
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
  FBSIMDArray<float, FFOsciFixedBlockOversamples> basicSqrPWPlain;
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
      if (_basicSinOn)
        basicSinGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicSinGain, basicSinGainNorm, s));
      if (_basicSawOn)
        basicSawGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicSawGain, basicSawGainNorm, s));
      if (_basicTriOn)
        basicTriGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicTriGain, basicTriGainNorm, s));
      if (_basicSqrOn)
      {
        basicSqrPWPlain.Store(s, topo.NormalizedToIdentityFast(FFOsciParam::BasicSqrPW, basicSqrPWNorm, s));
        basicSqrGainPlain.Store(s, topo.NormalizedToLinearFast(FFOsciParam::BasicSqrGain, basicSqrGainNorm, s));
      }
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
      if (_basicSinOn)
        basicSinGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      if (_basicSawOn)
        basicSawGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      if (_basicTriOn)
        basicTriGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      if (_basicSqrOn)
      {
        basicSqrPWPlain.UpsampleStretch<FFOsciOversamplingTimes>();
        basicSqrGainPlain.UpsampleStretch<FFOsciOversamplingTimes>();
      }
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
          FBSIMDVector<float> saw;
          if (_basicSawOn || _basicSqrOn)
            saw = GenerateSaw(uniPhase, uniIncr);
          if (_basicSawOn)
          {
            auto basicSawGain = basicSawGainPlain.Load(s);
            thisUniOutput += saw * basicSawGain;
          }
          if (_basicSinOn)
          {
            auto basicSinGain = basicSinGainPlain.Load(s);
            thisUniOutput += xsimd::sin(uniPhase * FBTwoPi) * basicSinGain;
          }
          if (_basicTriOn)
          {
            auto basicTriGain = basicTriGainPlain.Load(s);
            thisUniOutput += GenerateTri(uniPhase, uniIncr) * basicTriGain;
          }
          if (_basicSqrOn)
          {
            auto basicSqrPW = basicSqrPWPlain.Load(s);
            auto basicSqrGain = basicSqrGainPlain.Load(s);
            thisUniOutput += GenerateSqr(saw, uniPhase, uniIncr, basicSqrPW) * basicSqrGain;
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

  // todo get the fm matrix in here
  // todo expo fm
  if (_type == FFOsciType::FM)
  {
    for (int s = 0; s < totalSamples; s++)
      for (int u = 0; u < _uniCount; u += FBSIMDFloatCount)
      {
        int block = u / FBSIMDFloatCount;
        auto uniPosMHalfToHalf = _uniPosMHalfToHalf.Load(u);
        FBSIMDVector<float> op3UniPitch = basePitchPlain.Get(s) + uniPosMHalfToHalf * uniDetunePlain.Get(s);
        auto op3UniFreq = FBPitchToFreq(op3UniPitch);
        auto op2UniFreq = op3UniFreq / fmRatioPlain[1].Get(s);
        auto op1UniFreq = op2UniFreq / fmRatioPlain[0].Get(s);
        auto op1UniIncr = op1UniFreq / oversampledRate;
        auto op2UniIncr = op2UniFreq / oversampledRate;
        auto op3UniIncr = op3UniFreq / oversampledRate;

        FBSIMDVector<float> fmTo1 = 0.0f;
        fmTo1 += fmIndexPlain[0].Get(s) * _prevUniFMOutput[0].Load(u);
        fmTo1 += fmIndexPlain[3].Get(s) * _prevUniFMOutput[1].Load(u);
        fmTo1 += fmIndexPlain[6].Get(s) * _prevUniFMOutput[2].Load(u);
        auto output1 = xsimd::sin(_uniFMPhaseGens[0][block].Next(op1UniIncr, fmTo1) * FBTwoPi);

        FBSIMDVector<float> fmTo2 = 0.0f;
        fmTo2 += fmIndexPlain[1].Get(s) * output1;
        fmTo2 += fmIndexPlain[4].Get(s) * _prevUniFMOutput[1].Load(u);
        fmTo2 += fmIndexPlain[7].Get(s) * _prevUniFMOutput[2].Load(u);
        auto output2 = xsimd::sin(_uniFMPhaseGens[1][block].Next(op2UniIncr, fmTo2) * FBTwoPi);

        FBSIMDVector<float> fmTo3 = 0.0f;
        fmTo3 += fmIndexPlain[2].Get(s) * output1;
        fmTo3 += fmIndexPlain[5].Get(s) * output2;
        fmTo3 += fmIndexPlain[8].Get(s) * _prevUniFMOutput[2].Load(u);
        auto output3 = xsimd::sin(_uniFMPhaseGens[2][block].Next(op3UniIncr, fmTo3) * FBTwoPi);

        _prevUniFMOutput[0].Store(u, output1);
        _prevUniFMOutput[1].Store(u, output2);
        _prevUniFMOutput[2].Store(u, output3);

        FBSIMDArray<float, FBSIMDFloatCount> outputArray;
        outputArray.Store(0, output3);
        for (int v = 0; v < FBSIMDFloatCount; v++)
          uniOutputOversampled[u + v].Set(s, outputArray.Get(v));
      }
  }

#if 0
        auto uniPhase = _uniPhaseGens[u].Next(uniIncr, matrixFMMod * applyModMatrixLinearFM);

        xsimd::batch<float, FBXSIMDBatchType> fmTo1 = 0.0f;
        fmTo1 += fmIndexPlain[0][nonOversampledIndex] * _prevUniOutputForFM[0][subUniBlock];
        fmTo1 += fmIndexPlain[3][nonOversampledIndex] * _prevUniOutputForFM[1][subUniBlock];
        fmTo1 += fmIndexPlain[6][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto output1 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 0,
          _uniPitchesForFM[0][os][s].data() + u,
          _uniIncrsForFM[0][os][s].data() + u, fmTo1,
          0.0f);

        xsimd::batch<float, FBXSIMDBatchType> fmTo2 = 0.0f;
        fmTo2 += fmIndexPlain[1][nonOversampledIndex] * output1;
        fmTo2 += fmIndexPlain[4][nonOversampledIndex] * _prevUniOutputForFM[1][subUniBlock];
        fmTo2 += fmIndexPlain[7][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto output2 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 1,
          _uniPitchesForFM[1][os][s].data() + u,
          _uniIncrsForFM[1][os][s].data() + u, fmTo2,
          0.0f);

        // op3 is output, it also takes the external inter-osci fm
        xsimd::batch<float, FBXSIMDBatchType> fmTo3 = 0.0f;
        fmTo3 += fmIndexPlain[2][nonOversampledIndex] * output1;
        fmTo3 += fmIndexPlain[5][nonOversampledIndex] * output2;
        fmTo3 += fmIndexPlain[8][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto externalFMModulatorsForFMBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(_externalFMModulatorsForFM[os][s].data() + u);
        auto output3 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 2,
          _uniPitchesForFM[2][os][s].data() + u,
          _uniIncrsForFM[2][os][s].data() + u, fmTo3,
          externalFMModulatorsForFMBatch);

        _prevUniOutputForFM[0][subUniBlock] = output1;
        _prevUniOutputForFM[1][subUniBlock] = output2;
        _prevUniOutputForFM[2][subUniBlock] = output3;

        alignas(FBSIMDAlign) std::array<float, FBSIMDFloatCount> outputArray;
        output3.store_aligned(outputArray.data());
        for (int v = 0; v < FBSIMDFloatCount; v++)
          unisonOutputMaybeOversampled[u + v][os][s] = outputArray[v];
#endif

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
  exchangeParams.acc.basicSqrPW[0][voice] = basicSqrPWNorm.Last();
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