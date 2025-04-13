#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

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

static inline float
GenerateDSF(
  float phase, float freq, float decay,
  float distFreq, int overtones)
{
  float const decayRange = 0.99f;
  float const scaleFactor = 0.975f;

  float n = static_cast<float>(overtones);
  float w = decay * decayRange;
  float wPowNp1 = FBFastPowFloatToInt(w, overtones + 1);
  float u = 2.0f * FBPi * phase;
  float v = 2.0f * FBPi * distFreq * phase / freq;
  float a = w * std::sin(u + n * v) - std::sin(u + (n + 1.0f) * v);
  float x = (w * std::sin(v - u) + std::sin(u)) + wPowNp1 * a;
  float y = 1.0f + w * w - 2.0f * w * std::cos(v);
  float scale = (1.0f - wPowNp1) / (1.0f - w);
  return x * scaleFactor / (y * scale);
}

static inline float
GenerateDSFOvertones(
  float phase, float freq, float decay,
  float distFreq, float maxOvertones, int overtones)
{
  overtones = std::min(overtones, FBFastFloor(maxOvertones));
  return GenerateDSF(phase, freq, decay, distFreq, overtones);
}

static inline float
GenerateDSFBandwidth(
  float phase, float freq, float decay,
  float distFreq, float maxOvertones, float bandwidth)
{
  int overtones = 1 + FBFastFloor(bandwidth * (maxOvertones - 1.0f));
  overtones = std::min(overtones, FBFastFloor(maxOvertones));
  return GenerateDSF(phase, freq, decay, distFreq, overtones);
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
  _voiceState.dsfBandwidthPlain = topo.NormalizedToIdentityFast(FFOsciParam::DSFBandwidth, params.block.dsfBandwidth[0].Voice()[voice]);
  _voiceState.fmExp = topo.NormalizedToBoolFast(FFOsciParam::FMExp, params.block.fmExp[0].Voice()[voice]);
  _voiceState.fmRatioMode = topo.NormalizedToListFast<FFOsciFMRatioMode>(FFOsciParam::FMRatioMode, params.block.fmRatioMode[0].Voice()[voice]);
  _voiceState.fmRatioRatio12 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, params.block.fmRatioRatio[0].Voice()[voice]));
  _voiceState.fmRatioRatio23 = FMRatioRatio(topo.NormalizedToDiscreteFast(FFOsciParam::FMRatioRatio, params.block.fmRatioRatio[1].Voice()[voice]));
  _voiceState.unisonCount = topo.NormalizedToDiscreteFast(FFOsciParam::UnisonCount, params.block.unisonCount[0].Voice()[voice]);
  _voiceState.unisonOffsetPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonOffset, params.block.unisonOffset[0].Voice()[voice]);
  _voiceState.unisonRandomPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonRandom, params.block.unisonRandom[0].Voice()[voice]);

  _phase = {};
  _prng = FBParkMillerPRNG(state.moduleSlot / static_cast<float>(FFOsciCount));
  for (int u = 0; u < _voiceState.unisonCount; u++)
  {
    float random = _voiceState.unisonRandomPlain;
    float unisonPhase = u * _voiceState.unisonOffsetPlain / _voiceState.unisonCount;
    float unisonRandom = ((1.0f - random) + random * _prng.Next()) * unisonPhase;
    for (int o = 0; o < FFOsciFMOperatorCount; o++)
    {
      _prevUnisonOutputForFM[u][o] = 0.0f;
      _unisonPhases[u][o] = FFOsciPhase(unisonRandom);
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

// todo split function
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

  // Only need to fill the oversampled version as it is the source of modulation.
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

  // osci core specific, non-oversampled
  FBFixedFloatArray dsfDecayPlain;
  FBFixedFloatArray basicSqrPWPlain;
  FBFixedFloatArray basicSinGainPlain;
  FBFixedFloatArray basicSawGainPlain;
  FBFixedFloatArray basicTriGainPlain;
  FBFixedFloatArray basicSqrGainPlain;
  std::array<FBFixedFloatArray, FFOsciFMMatrixSize> fmIndexPlain;
  std::array<FBFixedFloatArray, FFOsciFMOperatorCount - 1> fmRatioPlain;
  if (_voiceState.type == FFOsciType::Basic)
  {
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
  }
  else if (_voiceState.type == FFOsciType::DSF)
    topo.NormalizedToIdentityFast(FFOsciParam::DSFDecay, dsfDecayNorm, dsfDecayPlain);
  else if (_voiceState.type == FFOsciType::FM)
  {
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
        topo.NormalizedToLinearFast(FFOsciParam::FMRatioFree, fmRatioFreeNorm, fmRatioPlain[o]);
      }
  }

  // base pitch and freq, non-oversampled
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

  // unison detune and spread, non-oversampled
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

  // per-unison-voice pitch, freq and delta, non-oversampled
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> uniFreqs;
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> uniIncrs;
  for (int u = 0; u < _voiceState.unisonCount; u++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float uniPitch = basePitch[s] + unisonPos[u] * detunePlain[s];
      uniFreqs[u][s] = FBPitchToFreqFastAndInaccurate(uniPitch);
      uniIncrs[u][s] = uniFreqs[u][s] / oversampledRate;
    }

  // per-unison-voice external fm, oversampled
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
          for (int s = 0; s < FBFixedBlockSamples; s++)
            for (int os = 0; os < oversamplingTimes; os++)
              fmModulators[u][os][s] += fmModulatorBase[os][s] * fmIndex[s];
        else
          for (int s = 0; s < FBFixedBlockSamples; s++)
            for (int os = 0; os < oversamplingTimes; os++)
              fmModulators[u][os][s] += FBToUnipolar(fmModulatorBase[os][s]) * fmIndex[s];
      }

  // unison phase including external fm mod, oversampled
  // for dedicated fm-osci, we need to step 3 phases together, see below
  std::array<std::array<FBFixedFloatArray, FFOsciOverSamplingTimes>, FFOsciUnisonMaxCount> uniPhases;
  if (_voiceState.type == FFOsciType::Basic || _voiceState.type == FFOsciType::DSF)
    for (int u = 0; u < _voiceState.unisonCount; u++)
    {
      int oversampledIndex = 0;
      for (int os = 0; os < oversamplingTimes; os++)
        for (int s = 0; s < FBFixedBlockSamples; s++, oversampledIndex++)
          uniPhases[u][os][s] = _unisonPhases[u][0].Next(uniIncrs[u][oversampledIndex / oversamplingTimes], fmModulators[u][os][s]);
    }

  // per-unison-voice basic and dsf oscillator core, oversampled
  // basic and dsf should vectorize well this way
  if (_voiceState.type == FFOsciType::Basic)
  {
    if (_voiceState.basicSinOn)
      for (int u = 0; u < _voiceState.unisonCount; u++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int os = 0; os < oversamplingTimes; os++)
            unisonOutputMaybeOversampled[u][os][s] += GenerateSin(uniPhases[u][os][s]) * basicSinGainPlain[s];
    if (_voiceState.basicSawOn)
      for (int u = 0; u < _voiceState.unisonCount; u++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int os = 0; os < oversamplingTimes; os++)
            unisonOutputMaybeOversampled[u][os][s] += GenerateSaw(uniPhases[u][os][s], uniIncrs[u][s]) * basicSawGainPlain[s];
    if (_voiceState.basicTriOn)
      for (int u = 0; u < _voiceState.unisonCount; u++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int os = 0; os < oversamplingTimes; os++)
            unisonOutputMaybeOversampled[u][os][s] += GenerateTri(uniPhases[u][os][s], uniIncrs[u][s]) * basicTriGainPlain[s];
    if (_voiceState.basicSqrOn)
      for (int u = 0; u < _voiceState.unisonCount; u++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int os = 0; os < oversamplingTimes; os++)
            unisonOutputMaybeOversampled[u][os][s] += GenerateSqr(uniPhases[u][os][s], uniIncrs[u][s], basicSqrPWPlain[s]) * basicSqrGainPlain[s];
  } else if (_voiceState.type == FFOsciType::DSF)
  {
    // when oversampling, max overtones increases accordingly
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> dsfDistFreqs;
    std::array<FBFixedFloatArray, FFOsciUnisonMaxCount> dsfMaxOvertones;
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
      {
        dsfDistFreqs[u][s] = static_cast<float>(_voiceState.dsfDistance) * uniFreqs[u][s];
        dsfMaxOvertones[u][s] = (oversampledRate * 0.5f - uniFreqs[u][s]) / dsfDistFreqs[u][s];
      }

    if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
      for (int u = 0; u < _voiceState.unisonCount; u++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int os = 0; os < oversamplingTimes; os++)
            unisonOutputMaybeOversampled[u][os][s] = GenerateDSFOvertones(
              uniPhases[u][os][s], uniFreqs[u][s], dsfDecayPlain[s],
              dsfDistFreqs[u][s], dsfMaxOvertones[u][s], _voiceState.dsfOvertones);
    else if (_voiceState.dsfMode == FFOsciDSFMode::Bandwidth)
      for (int u = 0; u < _voiceState.unisonCount; u++)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          for (int os = 0; os < oversamplingTimes; os++)
            unisonOutputMaybeOversampled[u][os][s] = GenerateDSFBandwidth(
            uniPhases[u][os][s], uniFreqs[u][s], dsfDecayPlain[s],
            dsfDistFreqs[u][s], dsfMaxOvertones[u][s], _voiceState.dsfBandwidthPlain);
  }

  // per-unison-voice fm oscillator core, oversampled
  // there is a data dependency (because of feedback fm)
  // which prevents vectorization, so pull the unison loop
  // to inner and hope to get some simd out of it
  else if (_voiceState.type == FFOsciType::FM)
  {
    // dedicated 3-op fm osci with feedback, op3 is output
    std::array<std::array<FBFixedFloatArray, FFOsciFMOperatorCount - 1>, FFOsciUnisonMaxCount> uniIncrsOp1And2 = {};
    for (int u = 0; u < _voiceState.unisonCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
      {
        float op2Freq = uniFreqs[u][s] / fmRatioPlain[1][s];
        float op1Freq = op2Freq / fmRatioPlain[0][s];
        uniIncrsOp1And2[u][0][s] = op1Freq / oversampledRate;
        uniIncrsOp1And2[u][1][s] = op2Freq / oversampledRate;
      }

    int oversampledIndex = 0;
    for (int os = 0; os < oversamplingTimes; os++)
      for (int s = 0; s < FBFixedBlockSamples; s++, oversampledIndex++)
        for (int u = 0; u < _voiceState.unisonCount; u++)
        {
          int nonOversampledIndex = oversampledIndex / oversamplingTimes;

          float fmTo1 = 0.0f;
          fmTo1 += fmIndexPlain[0][nonOversampledIndex] * _prevUnisonOutputForFM[u][0];
          fmTo1 += fmIndexPlain[3][nonOversampledIndex] * _prevUnisonOutputForFM[u][1];
          fmTo1 += fmIndexPlain[6][nonOversampledIndex] * _prevUnisonOutputForFM[u][2];
          float phase1 = _unisonPhases[u][0].Next(uniIncrsOp1And2[u][0][nonOversampledIndex], fmModulators[u][os][s] + fmTo1);
          float output1 = GenerateSin(phase1);

          float fmTo2 = 0.0f;
          fmTo2 += fmIndexPlain[1][nonOversampledIndex] * output1;
          fmTo2 += fmIndexPlain[4][nonOversampledIndex] * _prevUnisonOutputForFM[u][1];
          fmTo2 += fmIndexPlain[7][nonOversampledIndex] * _prevUnisonOutputForFM[u][2];
          float phase2 = _unisonPhases[u][1].Next(uniIncrsOp1And2[u][1][nonOversampledIndex], fmModulators[u][os][s] + fmTo2);
          float output2 = GenerateSin(phase2);

          float fmTo3 = 0.0f;
          fmTo3 += fmIndexPlain[2][nonOversampledIndex] * output1;
          fmTo3 += fmIndexPlain[5][nonOversampledIndex] * output2;
          fmTo3 += fmIndexPlain[8][nonOversampledIndex] * _prevUnisonOutputForFM[u][2];
          float phase3 = _unisonPhases[u][2].Next(uniIncrs[u][nonOversampledIndex], fmModulators[u][os][s] + fmTo3);
          float output3 = GenerateSin(phase3);

          unisonOutputMaybeOversampled[u][os][s] = output3;
          _prevUnisonOutputForFM[u][0] = output1;
          _prevUnisonOutputForFM[u][1] = output2;
          _prevUnisonOutputForFM[u][2] = output3;
        }
  }

  // am modulation, oversampled
  for (int src = 0; src < state.moduleSlot; src++)
    for (int u = 0; u < _voiceState.unisonCount; u++)
      if (_voiceState.modSourceAMMode[src] != FFOsciModAMMode::Off && _voiceState.modSourceUnisonCount[src] > u)
      {
        int modSlot = OsciModStartSlot(state.moduleSlot) + src;
        auto const& amMix = procState->dsp.voice[voice].osciMod.outputAMMix[modSlot];
        auto const& rmModulator = procState->dsp.voice[voice].osci[src].unisonOutputMaybeOversampled[u];
        if (_voiceState.modSourceAMMode[src] == FFOsciModAMMode::RM)
          for (int s = 0; s < FBFixedBlockSamples; s++)
            for (int os = 0; os < oversamplingTimes; os++)
              unisonOutputMaybeOversampled[u][os][s] = 
                (1.0f - amMix[s]) * unisonOutputMaybeOversampled[u][os][s] + 
                amMix[s] * unisonOutputMaybeOversampled[u][os][s] * rmModulator[os][s];
        else
          for (int s = 0; s < FBFixedBlockSamples; s++)
            for (int os = 0; os < oversamplingTimes; os++)
              unisonOutputMaybeOversampled[u][os][s] = (1.0f - amMix[s]) * unisonOutputMaybeOversampled[u][os][s] + 
                amMix[s] * unisonOutputMaybeOversampled[u][os][s] * (rmModulator[os][s] * 0.5f + 0.5f);
      }  

  // downsample if oversampled, or just plain copy if not
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

  // apply unison stereo spread, non-oversampled
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