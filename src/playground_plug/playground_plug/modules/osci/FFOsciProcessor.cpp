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

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
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

// fixed overtone count limited by nyquist
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

// overtone count determined by available bandwidth between fundamental and nyquist
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
  // Just to get a hold of juce's internal oversampling buffers.
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

  _phaseGen = {};
  _prng = FBParkMillerPRNG(state.moduleSlot / static_cast<float>(FFOsciCount));

  // randomization to get rid of the phasing effect
  alignas(FBSIMDAlign) std::array<float, FFOsciUnisonMaxCount> unisonPhaseInit = {};
  for (int u = 0; u < _voiceState.unisonCount; u++)
  {
    float random = _voiceState.unisonRandomPlain;
    float unisonPhase = u * _voiceState.unisonOffsetPlain / _voiceState.unisonCount;
    unisonPhaseInit[u] = ((1.0f - random) + random * _prng.Next()) * unisonPhase;
    _uniPhaseGens[u] = FFOsciPhaseGenerator(unisonPhaseInit[u]);
  }

  // fm osci has it's own phase generators
  if (_voiceState.type == FFOsciType::FM)
  {
    for (int u = 0; u < _voiceState.unisonCount; u += FBSIMDFloatCount)
      for (int o = 0; o < FFOsciFMOperatorCount; o++)
      {
        _prevUniOutputForFM[o][u / FBSIMDFloatCount] = 0.0f;
        _uniPhaseGensForFM[o][u / FBSIMDFloatCount] = FFOsciFMPhasesGenerator(
          xsimd::batch<float, FBXSIMDBatchType>::load_aligned(unisonPhaseInit.data() + u));
      }
  }

  // need some info from the inter-osci mod matrix
  int modStartSlot = OsciModStartSlot(state.moduleSlot);
  auto const& modParams = procState->param.voice.osciMod[0];
  auto const& modTopo = state.topo->static_.modules[(int)FFModuleType::OsciMod];
  _voiceState.externalFMExp = modTopo.NormalizedToBoolFast(FFOsciModParam::ExpoFM, modParams.block.expoFM[0].Voice()[voice]);
  _voiceState.oversampling = modTopo.NormalizedToBoolFast(FFOsciModParam::Oversampling, modParams.block.oversampling[0].Voice()[voice]);
  for (int modSlot = modStartSlot; modSlot < modStartSlot + state.moduleSlot; modSlot++)
  {
    int srcOsciSlot = modSlot - modStartSlot;
    auto const& srcOsciParams = procState->param.voice.osci[srcOsciSlot];
    _voiceState.modSourceFMOn[srcOsciSlot] = modTopo.NormalizedToBoolFast(FFOsciModParam::FMOn, modParams.block.fmOn[modSlot].Voice()[voice]);
    _voiceState.modSourceUnisonCount[srcOsciSlot] = topo.NormalizedToDiscreteFast(FFOsciParam::UnisonCount, srcOsciParams.block.unisonCount[0].Voice()[voice]);
    _voiceState.modSourceAMMode[srcOsciSlot] = modTopo.NormalizedToListFast<FFOsciModAMMode>(FFOsciModParam::AMMode, modParams.block.amMode[modSlot].Voice()[voice]);
  }
}

// not per unison voice pitch and frequency, not oversampled
void
FFOsciProcessor::ProcessBasePitchAndFreq(
  FBStaticModule const& topo, float sampleRate,
  FBAccParamState const& coarseNorm, FBAccParamState const& fineNorm,
  FBFixedFloatArray& basePitch, FBFixedFloatArray& baseFreq)
{
  FBFixedFloatArray finePlain;
  FBFixedFloatArray coarsePlain;
  topo.NormalizedToLinearFast(FFOsciParam::Fine, fineNorm, finePlain);
  topo.NormalizedToLinearFast(FFOsciParam::Coarse, coarseNorm, coarsePlain);
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto finePlainBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(finePlain.Data().data() + s);
    auto coarsePlainBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(coarsePlain.Data().data() + s);
    auto pitchBatch = _voiceState.key + coarsePlainBatch + finePlainBatch;
    auto freqBatch = FBPitchToFreq(pitchBatch);
    pitchBatch.store_aligned(basePitch.Data().data() + s);
    freqBatch.store_aligned(baseFreq.Data().data() + s);
  }
  for (int s = 0; s < FBFixedBlockSamples; s ++)
    _phaseGen.Next(baseFreq[s] / sampleRate);
}

// continuous unison params and position, not oversampled
void
FFOsciProcessor::ProcessUniDetuneSpreadAndPos(
  FBStaticModule const& topo,
  FBAccParamState const& uniDetuneNorm,
  FBAccParamState const& uniSpreadNorm,
  FBFixedFloatArray& uniDetunePlain,
  FBFixedFloatArray& uniSpreadPlain,
  std::array<float, FFOsciUnisonMaxCount>& uniPositions)
{
  if (_voiceState.unisonCount == 1)
  {
    uniPositions[0] = 0.5f;
    uniDetunePlain.Fill(0.0f);
    uniSpreadPlain.Fill(0.0f);
    return;
  }
  topo.NormalizedToIdentityFast(FFOsciParam::UnisonDetune, uniDetuneNorm, uniDetunePlain);
  topo.NormalizedToIdentityFast(FFOsciParam::UnisonSpread, uniSpreadNorm, uniSpreadPlain);
  for (int u = 0; u < _voiceState.unisonCount; u++)
    uniPositions[u] = u / (_voiceState.unisonCount - 1.0f) - 0.5f;
}

// per unison voice freq and delta
void
FFOsciProcessor::ProcessUniFreqAndDelta(
  int oversamplingTimes,
  float oversampledRate)
{
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int os = 0; os < oversamplingTimes; os++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      {
        auto pitchBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(_uniPitches[u][os].Data().data() + s);
        auto freqBatch = FBPitchToFreq(pitchBatch);
        auto incrBatch = freqBatch / oversampledRate;
        freqBatch.store_aligned(_uniFreqs[u][os].Data().data() + s);
        incrBatch.store_aligned(_uniIncrs[u][os].Data().data() + s);
      }
}

// stereo-spread the unison voices
void
FFOsciProcessor::ProcessUniSpreadToStereo(
  FBFixedFloatArray const& uniSpreadPlain,
  std::array<float, FFOsciUnisonMaxCount> const& uniPositions,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> const& uniOutputNonOversampled,
  FBFixedFloatAudioArray& output)
{
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float uniPanning = 0.5f + uniPositions[u] * uniSpreadPlain[s];
      output[0][s] += (1.0f - uniPanning) * uniOutputNonOversampled[u][s];
      output[1][s] += uniPanning * uniOutputNonOversampled[u][s];
    }
}

// per unison voice phase, oversampled, for basic and dsf
// dedicated fm osci does it's own phase calculations
void
FFOsciProcessor::ProcessUniPhasesNonFM(
  int oversamplingTimes)
{
  float applyLinearFM = _voiceState.externalFMExp ? 0.0f : 1.0f;
  for (int u = 0; u < _voiceState.unisonCount; u++)
  {
    int oversampledIndex = 0;
    for (int os = 0; os < oversamplingTimes; os++)
      for (int s = 0; s < FBFixedBlockSamples; s++, oversampledIndex++)
        _uniPhases[u][os][s] = _uniPhaseGens[u].Next(
          _uniIncrs[u][os][s], _modMatrixFMModulators[u][os][s] * applyLinearFM);
  }
}

// per unison pitch, oversampled
// oversampling only kicks in for expo fm by mod matrix
void
FFOsciProcessor::ProcessUniPitches(
  int oversamplingTimes,
  FBFixedFloatArray const& basePitch,
  FBFixedFloatArray const& uniDetunePlain,
  std::array<float, FFOsciUnisonMaxCount> const& uniPositions)
{
  float applyExpoFM = _voiceState.externalFMExp ? 1.0f : 0.0f;
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int os = 0; os < oversamplingTimes; os++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
      {
        int nosIndex = (os * FBFixedBlockSamples + s) / oversamplingTimes;
        _uniPitches[u][os][s] = basePitch[nosIndex] + uniPositions[u] * uniDetunePlain[nosIndex];
        _uniPitches[u][os][s] += _modMatrixFMModulators[u][os][s] * _uniPitches[u][os][s] * applyExpoFM;
      }
}

// per unison voice stacked external FM modulators, oversampled
void
FFOsciProcessor::ProcessModMatrixFMModulators(
  int moduleSlot,
  int oversamplingTimes,
  std::array<FFOsciDSPState, FFOsciCount> const& allOsciDSPStates,
  std::array<FBFixedFloatArray, FFOsciModSlotCount> const& outputFMIndex)
{
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int os = 0; os < oversamplingTimes; os++)
      _modMatrixFMModulators[u][os].Fill(0.0f);
  for (int src = 0; src < moduleSlot; src++)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      if (_voiceState.modSourceFMOn[src] && _voiceState.modSourceUnisonCount[src] > u)
      {
        int modSlot = OsciModStartSlot(moduleSlot) + src;
        auto const& fmModulatorBase = allOsciDSPStates[src].unisonOutputMaybeOversampled[u];
        for (int os = 0; os < oversamplingTimes; os++)
          for (int s = 0; s < FBFixedBlockSamples; s++)
            _modMatrixFMModulators[u][os][s] += fmModulatorBase[os][s] * outputFMIndex[modSlot][s];
      }
}

// downsample if we generated at oversampled rate
void
FFOsciProcessor::ProcessDownSampling(
  int oversamplingTimes,
  FFOsciOversampledUnisonArray const& uniOutputMaybeOversampled,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount>& uniOutputNonOversampled)
{
  if (!_voiceState.oversampling)
  {
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
        uniOutputNonOversampled[u][s] = uniOutputMaybeOversampled[u][0][s];
  }
  else {
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          _oversampledBlock.setSample(u, os * FBFixedBlockSamples + s, uniOutputMaybeOversampled[u][os][s]);
    std::array<float*, FFOsciUnisonMaxCount> channelPointers = {};
    for (int u = 0; u < _voiceState.unisonCount; u++)
      channelPointers[u] = uniOutputNonOversampled[u].Data().data();
    AudioBlock<float> downsampled(channelPointers.data(), _voiceState.unisonCount, 0, FBFixedBlockSamples);
    _oversampling.processSamplesDown(downsampled);
  }
}

// per unison voice stacked external AM/RM modulators, oversampled
void
FFOsciProcessor::ProcessModMatrixAMModulators(
  int moduleSlot,
  int oversamplingTimes,
  std::array<FBFixedFloatArray, FFOsciModSlotCount> const& outputAMMix,
  std::array<FFOsciDSPState, FFOsciCount>& allOsciDSPStates)
{
  for (int src = 0; src < moduleSlot; src++)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      if (_voiceState.modSourceAMMode[src] != FFOsciModAMMode::Off && _voiceState.modSourceUnisonCount[src] > u)
      {
        int modSlot = OsciModStartSlot(moduleSlot) + src;
        auto& unisonOutputMaybeOversampled = allOsciDSPStates[moduleSlot].unisonOutputMaybeOversampled;
        auto const& rmModulator = allOsciDSPStates[src].unisonOutputMaybeOversampled[u];
        if (_voiceState.modSourceAMMode[src] == FFOsciModAMMode::RM)
          for (int os = 0; os < oversamplingTimes; os++)
            for (int s = 0; s < FBFixedBlockSamples; s++)
              unisonOutputMaybeOversampled[u][os][s] =
              (1.0f - outputAMMix[modSlot][s]) * unisonOutputMaybeOversampled[u][os][s] +
              outputAMMix[modSlot][s] * unisonOutputMaybeOversampled[u][os][s] * rmModulator[os][s];
        else
          for (int os = 0; os < oversamplingTimes; os++)
            for (int s = 0; s < FBFixedBlockSamples; s++)
              unisonOutputMaybeOversampled[u][os][s] = (1.0f - outputAMMix[modSlot][s]) * unisonOutputMaybeOversampled[u][os][s] +
              outputAMMix[modSlot][s] * unisonOutputMaybeOversampled[u][os][s] * (rmModulator[os][s] * 0.5f + 0.5f);
      }
}

// vectorization in the (over)sample dimension, 
// phases are precomputed so no data dependency in the time domain
void
FFOsciProcessor::ProcessBasic(
  FBModuleProcState& state,
  int oversamplingTimes)
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
          unisonOutputMaybeOversampled[u][os][s] += GenerateSin(_uniPhases[u][os][s]) * basicSinGainPlain[s];
  if (_voiceState.basicSawOn)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutputMaybeOversampled[u][os][s] += GenerateSaw(_uniPhases[u][os][s], _uniIncrs[u][os][s]) * basicSawGainPlain[s];
  if (_voiceState.basicTriOn)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutputMaybeOversampled[u][os][s] += GenerateTri(_uniPhases[u][os][s], _uniIncrs[u][os][s]) * basicTriGainPlain[s];
  if (_voiceState.basicSqrOn)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutputMaybeOversampled[u][os][s] += GenerateSqr(_uniPhases[u][os][s], _uniIncrs[u][os][s], basicSqrPWPlain[s]) * basicSqrGainPlain[s];
}

// vectorization in the (over)sample dimension, 
// phases are precomputed so no data dependency in the time domain
void
FFOsciProcessor::ProcessDSF(
  FBModuleProcState& state,
  int oversamplingTimes)
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

  // do NOT limit to nyquist by the oversampled rate
  // we want the maximum partial count to be equal
  // for both oversampled and non-oversampled cases
  FFOsciOversampledUnisonArray dsfDistFreqs;
  FFOsciOversampledUnisonArray dsfMaxOvertones;
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int os = 0; os < oversamplingTimes; os++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
      {
        dsfDistFreqs[u][os][s] = static_cast<float>(_voiceState.dsfDistance) * _uniFreqs[u][os][s];
        dsfMaxOvertones[u][os][s] = (sampleRate * 0.5f - _uniFreqs[u][os][s]) / dsfDistFreqs[u][os][s];
      }

  if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for(int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
          GenerateDSFOvertones(
            _uniPhases[u][os].Data().data() + s, 
            _uniFreqs[u][os].Data().data() + s,
            dsfDecayPlain.Data().data() + s,
            dsfDistFreqs[u][os].Data().data() + s,
            dsfMaxOvertones[u][os].Data().data() + s,
            _voiceState.dsfOvertones,
            unisonOutputMaybeOversampled[u][os].Data().data() + s);
  else if (_voiceState.dsfMode == FFOsciDSFMode::Bandwidth)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
          GenerateDSFBandwidth(
            _uniPhases[u][os].Data().data() + s,
            _uniFreqs[u][os].Data().data() + s,
            dsfDecayPlain.Data().data() + s,
            dsfDistFreqs[u][os].Data().data() + s,
            dsfMaxOvertones[u][os].Data().data() + s,
            _voiceState.dsfBandwidthPlain,
            unisonOutputMaybeOversampled[u][os].Data().data() + s);
}

// calculate 4 outputs in unison dimension for the fm generator
template <bool ExpoFM>
xsimd::batch<float, FBXSIMDBatchType> 
FFOsciProcessor::CalcOneSampleForFM(
  float oversampledRate, int subUniBlock, int op,
  float* uniPitchesForFM, float* uniIncrsForFM,
  xsimd::batch<float, FBXSIMDBatchType> fmToOp,
  xsimd::batch<float, FBXSIMDBatchType> externalFMModulatorsForFMBatch)
{
  if constexpr (ExpoFM)
  {
    auto uniPitchOpBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(uniPitchesForFM);
    uniPitchOpBatch += fmToOp * uniPitchOpBatch;
    auto uniPitchOpFreq = 440.0f * xsimd::pow(xsimd::batch<float, FBXSIMDBatchType>(2.0f), (uniPitchOpBatch - 69.0f) / 12.0f);
    auto uniIncrOpBatch = uniPitchOpFreq / oversampledRate;
    return xsimd::sin(_uniPhaseGensForFM[op][subUniBlock].Next(uniIncrOpBatch, 0.0f) * FBTwoPi);
  }
  else
  {
    auto uniIncrOpBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(uniIncrsForFM);
    return xsimd::sin(_uniPhaseGensForFM[op][subUniBlock].Next(uniIncrOpBatch, fmToOp) * FBTwoPi);
  }
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
  alignas(FBSIMDAlign) std::array<std::array<std::array<float,
    FFOsciUnisonMaxCount>, FBFixedBlockSamples>, FFOsciOverSamplingTimes> externalFMModulatorsForFM = {};
  for (int os = 0; os < oversamplingTimes; os++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      for (int u = 0; u < FFOsciUnisonMaxCount; u++)
        externalFMModulatorsForFM[os][s][u] = _modMatrixFMModulators[u][os][s] * applyExternalLinearFM;

  // calculate op1/2/3 delta according to 1:2 and 2:3 ratio for expo case (ratio applies to pitch)
  alignas(FBSIMDAlign) std::array<std::array<std::array<std::array<float,
    FFOsciUnisonMaxCount>, FBFixedBlockSamples>, FFOsciOverSamplingTimes>, FFOsciFMOperatorCount> uniPitchsForFM = {};
  if constexpr (ExpoFM)
  {
    for (int o = 0; o < FFOsciFMOperatorCount; o++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int u = 0; u < FFOsciUnisonMaxCount; u++)
            uniPitchsForFM[o][os][s][u] = 0.0f;
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
        {
          float op3Pitch = _uniPitches[u][os][s];
          float op2Pitch = op3Pitch / fmRatioPlain[1][s];
          float op1Pitch = op2Pitch / fmRatioPlain[0][s];
          uniPitchsForFM[0][os][s][u] = op1Pitch;
          uniPitchsForFM[1][os][s][u] = op2Pitch;
          uniPitchsForFM[2][os][s][u] = op3Pitch;
        }
  }

  // calculate op1/2/3 delta according to 1:2 and 2:3 ratio for linear case
  // for expo case, we have to do it inside the per-sample loop because ops modulate pitch
  alignas(FBSIMDAlign) std::array<std::array<std::array<std::array<float,
    FFOsciUnisonMaxCount>, FBFixedBlockSamples>, FFOsciOverSamplingTimes>, FFOsciFMOperatorCount> uniIncrsForFM = {};
  if constexpr (!ExpoFM)
  {
    for (int o = 0; o < FFOsciFMOperatorCount; o++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int u = 0; u < FFOsciUnisonMaxCount; u++)
            uniIncrsForFM[o][os][s][u] = 0.0f;
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
        {
          float op3Freq = _uniFreqs[u][os][s];
          float op2Freq = op3Freq / fmRatioPlain[1][s];
          float op1Freq = op2Freq / fmRatioPlain[0][s];
          uniIncrsForFM[0][os][s][u] = op1Freq / oversampledRate;
          uniIncrsForFM[1][os][s][u] = op2Freq / oversampledRate;
          uniIncrsForFM[2][os][s][u] = op3Freq / oversampledRate;
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
          uniPitchsForFM[0][os][s].data() + u, 
          uniIncrsForFM[0][os][s].data() + u, fmTo1, 
          0.0f);

        xsimd::batch<float, FBXSIMDBatchType> fmTo2 = 0.0f;
        fmTo2 += fmIndexPlain[1][nonOversampledIndex] * output1;
        fmTo2 += fmIndexPlain[4][nonOversampledIndex] * _prevUniOutputForFM[1][subUniBlock];
        fmTo2 += fmIndexPlain[7][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto output2 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 1,
          uniPitchsForFM[1][os][s].data() + u, 
          uniIncrsForFM[1][os][s].data() + u, fmTo2, 
          0.0f);

        // op3 is output, it also takes the external inter-osci fm
        xsimd::batch<float, FBXSIMDBatchType> fmTo3 = 0.0f;
        fmTo3 += fmIndexPlain[2][nonOversampledIndex] * output1;
        fmTo3 += fmIndexPlain[5][nonOversampledIndex] * output2;
        fmTo3 += fmIndexPlain[8][nonOversampledIndex] * _prevUniOutputForFM[2][subUniBlock];
        auto externalFMModulatorsForFMBatch = xsimd::batch<float, FBXSIMDBatchType>::load_aligned(externalFMModulatorsForFM[os][s].data() + u);
        auto output3 = CalcOneSampleForFM<ExpoFM>(
          oversampledRate, subUniBlock, 2,
          uniPitchsForFM[2][os][s].data() + u, 
          uniIncrsForFM[2][os][s].data() + u, fmTo3, 
          externalFMModulatorsForFMBatch);

        _prevUniOutputForFM[0][subUniBlock] = output1;
        _prevUniOutputForFM[1][subUniBlock] = output2;
        _prevUniOutputForFM[2][subUniBlock] = output3;

        alignas(FBSIMDAlign) std::array<float, FBSIMDFloatCount> outputArray;
        output3.store_aligned(outputArray.data());
        for(int v = 0; v < FBSIMDFloatCount; v++)
          unisonOutputMaybeOversampled[u + v][os][s] = outputArray[v];
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
  auto& unisonOutputMaybeOversampled = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutputMaybeOversampled;

  output.Fill(0.0f);
  int oversamplingTimes = 1;
  float oversampledRate = sampleRate;
  if (_voiceState.oversampling)
  {
    oversampledRate *= FFOsciOverSamplingTimes;
    oversamplingTimes = FFOsciOverSamplingTimes;
  }

  // these are the am/fm mod sources, so need to clear
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for(int os = 0; os < oversamplingTimes; os++)
      unisonOutputMaybeOversampled[u][os].Fill(0.0f);

  if (_voiceState.type == FFOsciType::Off)
    return 0;

  int prevPositionSamplesUpToFirstCycle = _phaseGen.PositionSamplesUpToFirstCycle();

  auto const& fineNorm = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNorm = procParams.acc.coarse[0].Voice()[voice];
  auto const& uniDetuneNorm = procParams.acc.unisonDetune[0].Voice()[voice];
  auto const& uniSpreadNorm = procParams.acc.unisonSpread[0].Voice()[voice];
  auto const& dsfDecayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  auto const& basicSqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& basicSqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];
  auto const& basicSinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];  

  // stacked FM modulators, oversampled per unison voice
  ProcessModMatrixFMModulators(
    state.moduleSlot,
    oversamplingTimes,
    procState->dsp.voice[voice].osci,
    procState->dsp.voice[voice].osciMod.outputFMIndex);

  // base pitch and freq without unison, not oversampled
  FBFixedFloatArray baseFreq;
  FBFixedFloatArray basePitch;
  ProcessBasePitchAndFreq(topo, sampleRate, coarseNorm, fineNorm, basePitch, baseFreq);

  // continuous unison params and positioning, not oversampled
  FBFixedFloatArray uniDetunePlain;
  FBFixedFloatArray uniSpreadPlain;
  std::array<float, FFOsciUnisonMaxCount> uniPositions;
  ProcessUniDetuneSpreadAndPos(topo, uniDetuneNorm, uniSpreadNorm, uniDetunePlain, uniSpreadPlain, uniPositions);

  // per unison voice pitch, oversampled
  // note expo fm modulates pitch not phase
  ProcessUniPitches(oversamplingTimes, basePitch, uniDetunePlain, uniPositions);

  // per unison voice freq and delta unless dedicated 
  // fm osci of exponential type, in which case osci 
  // has to repeat pitch-to-frequency calculations
  if (_voiceState.type == FFOsciType::Basic || _voiceState.type == FFOsciType::DSF || !_voiceState.fmExp)
    ProcessUniFreqAndDelta(oversamplingTimes, oversampledRate);

  // for basic and dsf we can now precalculate the phases
  // dedicated fm osci has to do so itself because feedback loops
  // note we apply linear fm here to phase, but fm-osci has to repeat 
  // that logic to apply external fm from the inter-osci matrix
  if (_voiceState.type == FFOsciType::Basic || _voiceState.type == FFOsciType::DSF)
    ProcessUniPhasesNonFM(oversamplingTimes);

  // run the core osci algo
  if (_voiceState.type == FFOsciType::Basic)
    ProcessBasic(state, oversamplingTimes);
  else if (_voiceState.type == FFOsciType::DSF)
    ProcessDSF(state, oversamplingTimes);
  else if (_voiceState.type == FFOsciType::FM)
    if(_voiceState.fmExp)
      ProcessFM<true>(state, oversamplingTimes, oversampledRate);
    else
      ProcessFM<false>(state, oversamplingTimes, oversampledRate);

  // apply AM/RM
  ProcessModMatrixAMModulators(
    state.moduleSlot, 
    oversamplingTimes, 
    procState->dsp.voice[voice].osciMod.outputAMMix,
    procState->dsp.voice[voice].osci);

  // downsample when we generated at oversampled freq, otherwise just plain copy
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> uniOutputNonOversampled;
  ProcessDownSampling(oversamplingTimes, unisonOutputMaybeOversampled, uniOutputNonOversampled);

  // stereo-spread the unison voices
  ProcessUniSpreadToStereo(uniSpreadPlain, uniPositions, uniOutputNonOversampled, output);

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
    return _phaseGen.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreq.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phaseGen.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.fine[0][voice] = fineNorm.Last();
  exchangeParams.acc.coarse[0][voice] = coarseNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainWithGLFOBlock.Last();
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGainNorm.Last();
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