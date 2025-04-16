#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

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

static inline float
GenerateSin(float phase)
{
  return std::sin(phase * FBTwoPi);
}

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
static inline float
GenerateSaw(float phase, float incr)
{
  float blep = 0.0f;
  float saw = phase * 2.0f - 1.0f;
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
  return saw - blep;
}

static inline float
GenerateSqr(float phase, float incr, float pw)
{
  float minPW = 0.05f;
  float realPW = (minPW + (1.0f - minPW) * pw) * 0.5f;
  float phase2 = phase + realPW;
  FBPhaseWrap(phase2);
  return (GenerateSaw(phase, incr) - GenerateSaw(phase2, incr)) * 0.5f;
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

static inline float
GenerateTri(float phase, float incr)
{
  float v = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
  v += GenerateBLAMP(phase, incr);
  v += GenerateBLAMP(1.0f - phase, incr);
  phase += 0.5f;
  FBPhaseWrap(phase);
  v -= GenerateBLAMP(phase, incr);
  v -= GenerateBLAMP(1.0f - phase, incr);
  return v;
}

static inline void
GenerateDSF(
  float const* phases, float const* freqs, float const* decays,
  float const* distFreqs, float const* overtones, float* outs)
{
  float const decayRange = 0.99f;
  float const scaleFactor = 0.975f;

  auto freqBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(freqs);
  auto decayBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(decays);
  auto phaseBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(phases);
  auto distFreqBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(distFreqs);
  auto overtonesBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(overtones);

  xsimd::batch<float, FBXSIMDBatchType> n = overtonesBatch;
  xsimd::batch<float, FBXSIMDBatchType> w = decayBatch * decayRange;
  xsimd::batch<float, FBXSIMDBatchType> wPowNp1 = xsimd::pow(w, overtonesBatch + 1.0f);
  xsimd::batch<float, FBXSIMDBatchType> u = 2.0f * FBPi * phaseBatch;
  xsimd::batch<float, FBXSIMDBatchType> v = 2.0f * FBPi * distFreqBatch * phaseBatch / freqBatch;
  xsimd::batch<float, FBXSIMDBatchType> a = w * xsimd::sin(u + n * v) - xsimd::sin(u + (n + 1.0f) * v);
  xsimd::batch<float, FBXSIMDBatchType> x = (w * xsimd::sin(v - u) + xsimd::sin(u)) + wPowNp1 * a;
  xsimd::batch<float, FBXSIMDBatchType> y = 1.0f + w * w - 2.0f * w * xsimd::cos(v);
  xsimd::batch<float, FBXSIMDBatchType> scale = (1.0f - wPowNp1) / (1.0f - w);
  auto outBatch = x * scaleFactor / (y * scale);
  outBatch.store_aligned(outs);
}

static inline void
GenerateDSFOvertones(
  float const* phases, float const* freqs, float const* decays,
  float const* distFreqs, float const* maxOvertones, int overtones_, float* outs)
{
  alignas(FBSIMDAlign) std::array<float, FBSIMDFloatCount> overtones;
  for(int i = 0; i < FBSIMDFloatCount; i++)
    overtones[i] = static_cast<float>(std::min(overtones_, FBFastFloor(maxOvertones[i])));
  return GenerateDSF(phases, freqs, decays, distFreqs, overtones.data(), outs);
}

static inline void
GenerateDSFBandwidth(
  float const* phases, float const* freqs, float const* decays,
  float const* distFreqs, float const* maxOvertones, float bandwidth, float* outs)
{
  alignas(FBSIMDAlign) std::array<float, FBSIMDFloatCount> overtones;
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    overtones[i] = 1.0f + FBFastFloor(bandwidth * (maxOvertones[i] - 1));
    overtones[i] = std::min(overtones[i], static_cast<float>(FBFastFloor(maxOvertones[i])));
  }
  return GenerateDSF(phases, freqs, decays, distFreqs, overtones.data(), outs);
}

FFOsciProcessor::
FFOsciProcessor():
_oversampling(
  FFOsciUnisonMaxCount, FFOsciOverSamplingFactor,
  Oversampling<float>::filterHalfBandPolyphaseIIR, 
  false, false)
{
  // Just to get a hold of juce's internal buffers.
  std::array<float*, FBFixedBlockSamples> dummyData = {};
  std::array<std::array<float, FBFixedBlockSamples>, FFOsciUnisonMaxCount> dummyChannelData = {};
  for (int u = 0; u < FFOsciUnisonMaxCount; u++)
    dummyData[u] = dummyChannelData[u].data();
  AudioBlock<float> dummyBlock(dummyData.data(), FFOsciUnisonMaxCount, 0, FBFixedBlockSamples);
  _oversampling.initProcessing(FBFixedBlockSamples);
  _oversampledBlock = _oversampling.processSamplesUp(dummyBlock);
}

void
FFOsciProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  _voiceState.key = static_cast<float>(state.voice->event.note.key);
  _voiceState.note = topo.NormalizedToNoteFast(FFOsciParam::Note, params.block.note[0].Voice()[voice]);
  _voiceState.type = topo.NormalizedToListFast<FFOsciType>(FFOsciParam::Type, params.block.type[0].Voice()[voice]);
  _voiceState.basicSinOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSinOn, params.block.basicSinOn[0].Voice()[voice]);
  _voiceState.basicSawOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSawOn, params.block.basicSawOn[0].Voice()[voice]);
  _voiceState.basicTriOn = topo.NormalizedToBoolFast(FFOsciParam::BasicTriOn, params.block.basicTriOn[0].Voice()[voice]);
  _voiceState.basicSqrOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSqrOn, params.block.basicSqrOn[0].Voice()[voice]);
  _voiceState.dsfMode = topo.NormalizedToListFast<FFOsciDSFMode>(FFOsciParam::DSFMode, params.block.dsfMode[0].Voice()[voice]);
  _voiceState.dsfDistance = topo.NormalizedToDiscreteFast(FFOsciParam::DSFDistance, params.block.dsfDistance[0].Voice()[voice]);
  _voiceState.dsfOvertones = topo.NormalizedToDiscreteFast(FFOsciParam::DSFOvertones, params.block.dsfOvertones[0].Voice()[voice]);
  _voiceState.dsfBandwidthPlain = topo.NormalizedToLog2Fast(FFOsciParam::DSFBandwidth, params.block.dsfBandwidth[0].Voice()[voice]);
  _voiceState.fmExp = topo.NormalizedToBoolFast(FFOsciParam::FMExp, params.block.fmExp[0].Voice()[voice]);
  _voiceState.fmRatioMode = topo.NormalizedToListFast<FFOsciFMRatioMode>(FFOsciParam::FMRatioMode, params.block.fmRatioMode[0].Voice()[voice]);
  _voiceState.fmRatioRatio12 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, params.block.fmRatioRatio[0].Voice()[voice]));
  _voiceState.fmRatioRatio23 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, params.block.fmRatioRatio[1].Voice()[voice]));
  _voiceState.unisonCount = topo.NormalizedToDiscreteFast(FFOsciParam::UnisonCount, params.block.unisonCount[0].Voice()[voice]);
  _voiceState.unisonOffsetPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonOffset, params.block.unisonOffset[0].Voice()[voice]);
  _voiceState.unisonRandomPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonRandom, params.block.unisonRandom[0].Voice()[voice]);

  _phase = {};
  _prng = FBParkMillerPRNG(state.moduleSlot / static_cast<float>(FFOsciCount));
  alignas(FBSIMDAlign) std::array<float, FFOsciUnisonMaxCount> unisonPhaseInit = {};
  for (int u = 0; u < _voiceState.unisonCount; u++)
  {
    float random = _voiceState.unisonRandomPlain;
    float unisonPhase = u * _voiceState.unisonOffsetPlain / _voiceState.unisonCount;
    unisonPhaseInit[u] = ((1.0f - random) + random * _prng.Next()) * unisonPhase;
    _unisonPhases[u] = FFOsciPhase(unisonPhaseInit[u]);
  }
  if (_voiceState.type == FFOsciType::FM)
  {
    for (int u = 0; u < _voiceState.unisonCount; u += FBSIMDFloatCount)
      for (int o = 0; o < FFOsciFMOperatorCount; o++)
      {
        _prevUnisonOutputForFM[o][u / FBSIMDFloatCount] = 0.0f;
        _unisonPhasesForFM[o][u / FBSIMDFloatCount] = FFOsciFMPhases(
          xsimd::batch<float, FBXSIMDBatchType>::load_aligned(unisonPhaseInit.data() + u));
      }
  }

  int modStartSlot = OsciModStartSlot(state.moduleSlot);
  auto const& modParams = procState->param.voice.osciMod[0];
  auto const& modTopo = state.topo->static_.modules[(int)FFModuleType::OsciMod];
  _voiceState.oversampling = modTopo.NormalizedToBoolFast(FFOsciModParam::Oversampling, modParams.block.oversampling[0].Voice()[voice]);
  for (int modSlot = modStartSlot; modSlot < modStartSlot + state.moduleSlot; modSlot++)
  {
    int srcOsciSlot = modSlot - modStartSlot;
    auto const& srcOsciParams = procState->param.voice.osci[srcOsciSlot];
    _voiceState.modSourceUnisonCount[srcOsciSlot] = topo.NormalizedToDiscreteFast(FFOsciParam::UnisonCount, srcOsciParams.block.unisonCount[0].Voice()[voice]);
    _voiceState.modSourceAMMode[srcOsciSlot] = modTopo.NormalizedToListFast<FFOsciModAMMode>(FFOsciModParam::AMMode, modParams.block.amMode[modSlot].Voice()[voice]);
    _voiceState.modSourceFMMode[srcOsciSlot] = modTopo.NormalizedToListFast<FFOsciModFMMode>(FFOsciModParam::FMMode, modParams.block.fmMode[modSlot].Voice()[voice]);
  }
}

void
FFOsciProcessor::ProcessBasic(
  FBModuleProcState& state,
  int oversamplingTimes,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniIncrs,
  std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> const& uniPhases)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& unisonOutputMaybeOversampled = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutputMaybeOversampled;

  auto const& basicSqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& basicSqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];
  auto const& basicSinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];

  FBFixedFloatArray basicSqrPWPlain;
  FBFixedFloatArray basicSinGainPlain;
  FBFixedFloatArray basicSawGainPlain;
  FBFixedFloatArray basicTriGainPlain;
  FBFixedFloatArray basicSqrGainPlain;
  if (_voiceState.basicSinOn)
    topo.NormalizedToLinearFast(FFOsciParam::BasicSinGain, basicSinGainNorm, basicSinGainPlain);
  if (_voiceState.basicSawOn)
    topo.NormalizedToLinearFast(FFOsciParam::BasicSawGain, basicSawGainNorm, basicSawGainPlain);
  if (_voiceState.basicTriOn)
    topo.NormalizedToLinearFast(FFOsciParam::BasicTriGain, basicTriGainNorm, basicTriGainPlain);
  if (_voiceState.basicSqrOn)
  {
    topo.NormalizedToIdentityFast(FFOsciParam::BasicSqrPW, basicSqrPWNorm, basicSqrPWPlain);
    topo.NormalizedToLinearFast(FFOsciParam::BasicSqrGain, basicSqrGainNorm, basicSqrGainPlain);
  }

  if (_voiceState.basicSinOn)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutputMaybeOversampled[u][os][s] += GenerateSin(uniPhases[u][os][s]) * basicSinGainPlain[s];
  if (_voiceState.basicSawOn)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutputMaybeOversampled[u][os][s] += GenerateSaw(uniPhases[u][os][s], uniIncrs[u][s]) * basicSawGainPlain[s];
  if (_voiceState.basicTriOn)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutputMaybeOversampled[u][os][s] += GenerateTri(uniPhases[u][os][s], uniIncrs[u][s]) * basicTriGainPlain[s];
  if (_voiceState.basicSqrOn)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutputMaybeOversampled[u][os][s] += GenerateSqr(uniPhases[u][os][s], uniIncrs[u][s], basicSqrPWPlain[s]) * basicSqrGainPlain[s];
}

void
FFOsciProcessor::ProcessDSF(
  FBModuleProcState& state,
  int oversamplingTimes,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniFreqs,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniIncrs,
  std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> const& uniPhases)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& unisonOutputMaybeOversampled = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutputMaybeOversampled;

  FBFixedFloatArray dsfDecayPlain;
  auto const& dsfDecayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  topo.NormalizedToIdentityFast(FFOsciParam::DSFDecay, dsfDecayNorm, dsfDecayPlain);

  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> dsfDistFreqs;
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> dsfMaxOvertones;
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      dsfDistFreqs[u][s] = static_cast<float>(_voiceState.dsfDistance) * uniFreqs[u][s];
      dsfMaxOvertones[u][s] = (sampleRate * 0.5f - uniFreqs[u][s]) / dsfDistFreqs[u][s];
    }

  if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for(int subBlock = 0; subBlock < FBFixedBlockSamples / FBSIMDFloatCount; subBlock++)
          GenerateDSFOvertones(
            uniPhases[u][os].Data().data() + subBlock * FBSIMDFloatCount, 
            uniFreqs[u].Data().data() + subBlock * FBSIMDFloatCount,
            dsfDecayPlain.Data().data() + subBlock * FBSIMDFloatCount,
            dsfDistFreqs[u].Data().data() + subBlock * FBSIMDFloatCount,
            dsfMaxOvertones[u].Data().data() + subBlock * FBSIMDFloatCount,
            _voiceState.dsfOvertones,
            unisonOutputMaybeOversampled[u][os].Data().data() + subBlock * FBSIMDFloatCount);
  else if (_voiceState.dsfMode == FFOsciDSFMode::Bandwidth)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int subBlock = 0; subBlock < FBFixedBlockSamples / FBSIMDFloatCount; subBlock++)
          GenerateDSFBandwidth(
            uniPhases[u][os].Data().data() + subBlock * FBSIMDFloatCount,
            uniFreqs[u].Data().data() + subBlock * FBSIMDFloatCount,
            dsfDecayPlain.Data().data() + subBlock * FBSIMDFloatCount,
            dsfDistFreqs[u].Data().data() + subBlock * FBSIMDFloatCount,
            dsfMaxOvertones[u].Data().data() + subBlock * FBSIMDFloatCount,
            _voiceState.dsfBandwidthPlain,
            unisonOutputMaybeOversampled[u][os].Data().data() + subBlock * FBSIMDFloatCount);
}

void
FFOsciProcessor::ProcessFM(
  FBModuleProcState& state,
  int oversamplingTimes,
  float oversampledRate,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniFreqs,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniIncrs,
  std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> const& fmModulators)
{
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

  alignas(FBSIMDAlign) std::array<std::array<std::array<float, 
    FFOsciUnisonMaxCount>, FBFixedBlockSamples>, FFOsciFMOperatorCount> uniIncrsForFM;
  for (int s = 0; s < FBFixedBlockSamples; s++)
    for (int u = 0; u < _voiceState.unisonCount; u++)
    {
      float op3Freq = uniFreqs[u][s];
      float op2Freq = op3Freq / fmRatioPlain[1][s];
      float op1Freq = op2Freq / fmRatioPlain[0][s];
      uniIncrsForFM[0][s][u] = op1Freq / oversampledRate;
      uniIncrsForFM[1][s][u] = op2Freq / oversampledRate;
      uniIncrsForFM[2][s][u] = op3Freq / oversampledRate;
    }

  // data dependency in the feedback loop,
  // need to vectorize in the unison dimension
  int oversampledIndex = 0;
  for (int os = 0; os < oversamplingTimes; os++)
    for (int s = 0; s < FBFixedBlockSamples; s++, oversampledIndex++)
    {
      int nonOversampledIndex = oversampledIndex / oversamplingTimes;
      for (int u = 0; u < _voiceState.unisonCount; u += FBSIMDFloatCount)
      {
        int subUniBlock = u / FBSIMDFloatCount;

        xsimd::batch<float, FBXSIMDBatchType> fmTo1 = 0.0f;
        fmTo1 += fmIndexPlain[0][nonOversampledIndex] * _prevUnisonOutputForFM[0][subUniBlock];
        fmTo1 += fmIndexPlain[3][nonOversampledIndex] * _prevUnisonOutputForFM[1][subUniBlock];
        fmTo1 += fmIndexPlain[6][nonOversampledIndex] * _prevUnisonOutputForFM[2][subUniBlock];
        auto uniIncrOp1Batch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(uniIncrsForFM[0][nonOversampledIndex].data() + u);
        auto phase1 = _unisonPhasesForFM[0][subUniBlock].Next(uniIncrOp1Batch, 0.0f); // todo fmModulators[u][os][s] + fmTo1);
        auto output1 = xsimd::sin(phase1);

        xsimd::batch<float, FBXSIMDBatchType> fmTo2 = 0.0f;
        fmTo2 += fmIndexPlain[1][nonOversampledIndex] * output1;
        fmTo2 += fmIndexPlain[4][nonOversampledIndex] * _prevUnisonOutputForFM[1][subUniBlock];
        fmTo2 += fmIndexPlain[7][nonOversampledIndex] * _prevUnisonOutputForFM[2][subUniBlock];
        auto uniIncrOp2Batch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(uniIncrsForFM[1][nonOversampledIndex].data() + u);
        auto phase2 = _unisonPhasesForFM[1][subUniBlock].Next(uniIncrOp2Batch, 0.0f); // todo fmModulators[u][os][s] + fmTo1);
        auto output2 = xsimd::sin(phase2);

        xsimd::batch<float, FBXSIMDBatchType> fmTo3 = 0.0f;
        fmTo3 += fmIndexPlain[2][nonOversampledIndex] * output1;
        fmTo3 += fmIndexPlain[5][nonOversampledIndex] * output2;
        fmTo3 += fmIndexPlain[8][nonOversampledIndex] * _prevUnisonOutputForFM[2][subUniBlock];
        auto uniIncrOp3Batch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(uniIncrsForFM[2][nonOversampledIndex].data() + u);
        auto phase3 = _unisonPhasesForFM[2][subUniBlock].Next(uniIncrOp3Batch, 0.0f); // todo fmModulators[u][os][s] + fmTo1);
        auto output3 = xsimd::sin(phase3);

        _prevUnisonOutputForFM[0][subUniBlock] = output1;
        _prevUnisonOutputForFM[1][subUniBlock] = output2;
        _prevUnisonOutputForFM[2][subUniBlock] = output3;

        //unisonOutputMaybeOversampled[u][os][s] = output3; // todo
      }
    }
}

int
FFOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& output = procState->dsp.voice[voice].osci[state.moduleSlot].output;
  auto& unisonOutputNonOversampled = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutputNonOversampled;
  auto& unisonOutputMaybeOversampled = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutputMaybeOversampled;

  output.Fill(0.0f);
  int oversamplingTimes = 1;
  float oversampledRate = sampleRate;
  if (_voiceState.oversampling)
  {
    oversampledRate *= FFOsciOverSamplingTimes;
    oversamplingTimes = FFOsciOverSamplingTimes;
  }

  for (int u = 0; u < _voiceState.unisonCount; u++)
    for(int os = 0; os < oversamplingTimes; os++)
      unisonOutputMaybeOversampled[u][os].Fill(0.0f);

  if (_voiceState.type == FFOsciType::Off)
    return 0;

  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();

  auto const& dsfDecayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  auto const& detuneNorm = procParams.acc.unisonDetune[0].Voice()[voice];
  auto const& spreadNorm = procParams.acc.unisonSpread[0].Voice()[voice];
  auto const& basicSqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& basicSqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];
  auto const& basicSinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];  

  float centNorm;
  FBFixedFloatArray baseFreq;
  FBFixedFloatArray basePitch;
  float notePitch = _voiceState.key + _voiceState.note - 60.0f;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    centNorm = procParams.acc.cent[0].Voice()[voice].CV()[s];
    float centPlain = topo.NormalizedToLinearFast(FFOsciParam::Cent, centNorm);
    basePitch[s] = notePitch + centPlain;
    baseFreq[s] = FBPitchToFreqFastAndInaccurate(basePitch[s]);
    _phase.Next(baseFreq[s] / state.input->sampleRate);
  }

  FBFixedFloatArray detunePlain;
  FBFixedFloatArray spreadPlain;
  std::array<float, FFOsciUnisonMaxCount> unisonPos;
  if (_voiceState.unisonCount == 1)
  {
    unisonPos[0] = 0.5f;
    detunePlain.Fill(0.0f);
    spreadPlain.Fill(0.0f);
  }
  else
  {
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonDetune, detuneNorm, detunePlain);
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonSpread, spreadNorm, spreadPlain);
    for(int u = 0; u < _voiceState.unisonCount; u++)
      unisonPos[u] = u / (_voiceState.unisonCount - 1.0f) - 0.5f;
  }

  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> uniFreqs;
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> uniIncrs;
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float uniPitch = basePitch[s] + unisonPos[u] * detunePlain[s];
      uniFreqs[u][s] = FBPitchToFreqFastAndInaccurate(uniPitch);
      uniIncrs[u][s] = uniFreqs[u][s] / oversampledRate;
    }

  std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> fmModulators;
  for(int u = 0; u < _voiceState.unisonCount; u++)
    for (int os = 0; os < oversamplingTimes; os++)
      fmModulators[u][os].Fill(0.0f);
  for (int src = 0; src < state.moduleSlot; src++)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      if (_voiceState.modSourceFMMode[src] != FFOsciModFMMode::Off && _voiceState.modSourceUnisonCount[src] > u)
      {
        int modSlot = OsciModStartSlot(state.moduleSlot) + src;
        auto const& fmIndex = procState->dsp.voice[voice].osciMod.outputFMIndex[modSlot];
        auto const& fmModulatorBase = procState->dsp.voice[voice].osci[src].unisonOutputMaybeOversampled[u];
        if (_voiceState.modSourceFMMode[src] == FFOsciModFMMode::TZ)
          for (int os = 0; os < oversamplingTimes; os++)
            for (int s = 0; s < FBFixedBlockSamples; s++)
              fmModulators[u][os][s] += fmModulatorBase[os][s] * fmIndex[s];
        else
          for (int os = 0; os < oversamplingTimes; os++)
            for (int s = 0; s < FBFixedBlockSamples; s++)
              fmModulators[u][os][s] += FBToUnipolar(fmModulatorBase[os][s]) * fmIndex[s];
      }

  std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> uniPhases;
  if (_voiceState.type == FFOsciType::Basic || _voiceState.type == FFOsciType::DSF)
    for (int u = 0; u < _voiceState.unisonCount; u++)
    {
      int oversampledIndex = 0;
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++, oversampledIndex++)
          uniPhases[u][os][s] = _unisonPhases[u].Next(uniIncrs[u][oversampledIndex / oversamplingTimes], fmModulators[u][os][s]);
    }

  if (_voiceState.type == FFOsciType::Basic)
    ProcessBasic(state, oversamplingTimes, uniIncrs, uniPhases);
  else if (_voiceState.type == FFOsciType::DSF)
    ProcessDSF(state, oversamplingTimes, uniFreqs, uniIncrs, uniPhases);
  else if (_voiceState.type == FFOsciType::FM)
    ProcessFM(state, oversamplingTimes, oversampledRate, uniFreqs, uniIncrs, fmModulators);

  for (int src = 0; src < state.moduleSlot; src++)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      if (_voiceState.modSourceAMMode[src] != FFOsciModAMMode::Off && _voiceState.modSourceUnisonCount[src] > u)
      {
        int modSlot = OsciModStartSlot(state.moduleSlot) + src;
        auto const& amMix = procState->dsp.voice[voice].osciMod.outputAMMix[modSlot];
        auto const& rmModulator = procState->dsp.voice[voice].osci[src].unisonOutputMaybeOversampled[u];
        if (_voiceState.modSourceAMMode[src] == FFOsciModAMMode::RM)
          for (int os = 0; os < oversamplingTimes; os++)
            for (int s = 0; s < FBFixedBlockSamples; s++)
              unisonOutputMaybeOversampled[u][os][s] = 
                (1.0f - amMix[s]) * unisonOutputMaybeOversampled[u][os][s] + 
                amMix[s] * unisonOutputMaybeOversampled[u][os][s] * rmModulator[os][s];
        else
          for (int os = 0; os < oversamplingTimes; os++)
            for (int s = 0; s < FBFixedBlockSamples; s++)
              unisonOutputMaybeOversampled[u][os][s] = (1.0f - amMix[s]) * unisonOutputMaybeOversampled[u][os][s] + 
                amMix[s] * unisonOutputMaybeOversampled[u][os][s] * (rmModulator[os][s] * 0.5f + 0.5f);
      }  

  if (!_voiceState.oversampling)
  {
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
        unisonOutputNonOversampled[u][s] = unisonOutputMaybeOversampled[u][0][s];
  } else {
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          _oversampledBlock.setSample(u, os * FBFixedBlockSamples + s, unisonOutputMaybeOversampled[u][os][s]);
    std::array<float*, FFOsciUnisonMaxCount> channelPointers = {};
    for (int u = 0; u < _voiceState.unisonCount; u++)
      channelPointers[u] = unisonOutputNonOversampled[u].Data().data();
    AudioBlock<float> downsampled(channelPointers.data(), _voiceState.unisonCount, 0, FBFixedBlockSamples);
    _oversampling.processSamplesDown(downsampled);
  }

  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float uniPanning = 0.5f + unisonPos[u] * spreadPlain[s];
      output[0][s] += (1.0f - uniPanning) * unisonOutputNonOversampled[u][s];
      output[1][s] += uniPanning * unisonOutputNonOversampled[u][s];
    }

  FBFixedFloatArray gainPlain;
  FBFixedFloatArray gLFOToGainPlain;
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& gLFOToGainNorm = procParams.acc.gLFOToGain[0].Voice()[voice];
  topo.NormalizedToIdentityFast(FFOsciParam::Gain, gainNorm, gainPlain);
  topo.NormalizedToIdentityFast(FFOsciParam::GLFOToGain, gLFOToGainNorm, gLFOToGainPlain);

  // TODO this might prove difficult, lets see how it fares with the matrices
  FBFixedFloatArray gLFO;
  auto* exchangeFromGUI = state.ExchangeFromGUIAs<FFExchangeState>();
  if (exchangeFromGUI != nullptr)
    gLFO.Fill(exchangeFromGUI->global.gLFO[0].lastOutput);
  else
    procState->dsp.global.gLFO[0].output.CopyTo(gLFO);

  FBFixedFloatArray gainWithGLFOBlock;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    gainWithGLFOBlock[s] = ((1.0f - gLFOToGainPlain[s]) + gLFOToGainPlain[s] * gLFO[s]) * gainPlain[s];
    output[0][s] *= gainWithGLFOBlock[s];
    output[1][s] *= gainWithGLFOBlock[s];
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreq.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.cent[0][voice] = centNorm;
  exchangeParams.acc.gain[0][voice] = gainWithGLFOBlock.Last();
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGainNorm.Last();
  exchangeParams.acc.unisonDetune[0][voice] = detuneNorm.Last();
  exchangeParams.acc.unisonSpread[0][voice] = spreadNorm.Last();
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