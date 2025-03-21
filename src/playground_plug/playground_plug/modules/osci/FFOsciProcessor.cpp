#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

static FBFloatVector
GenerateSin(FBFloatVector phase)
{
  return xsimd::sin(phase * FBTwoPi);
}

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
static FBFloatVector
GenerateSaw(FBFloatVector phase, FBFloatVector incr)
{ 
  FBFloatVector one = 1.0f;
  FBFloatVector zero = 0.0f;
  FBFloatVector blepLo = phase / incr;
  FBFloatVector blepHi = (phase - 1.0f) / incr;
  FBFloatVector result = phase * 2.0f - 1.0f;
  auto loMask = xsimd::lt(phase, incr);
  auto hiMask = xsimd::ge(phase, 1.0f - incr);
  hiMask = xsimd::bitwise_andnot(hiMask, loMask);
  FBFloatVector loMul = xsimd::select(loMask, one, zero);
  FBFloatVector hiMul = xsimd::select(hiMask, one, zero);
  result -= loMul * ((2.0f - blepLo) * blepLo - 1.0f);
  result -= hiMul * ((blepHi + 2.0f) * blepHi + 1.0f);
  return result;
}

static FBFloatVector
GenerateSqr(FBFloatVector phase, FBFloatVector incr, FBFloatVector pw)
{
  FBFloatVector minPW = 0.05f;
  FBFloatVector realPW = (minPW + (1.0f - minPW) * pw) * 0.5f;
  FBFloatVector phase2 = phase + realPW;
  phase2 -= xsimd::floor(phase2);
  return (GenerateSaw(phase, incr) - GenerateSaw(phase2, incr)) * 0.5f;
}

// https://dsp.stackexchange.com/questions/54790/polyblamp-anti-aliasing-in-c
static FBFloatVector 
GenerateBLAMP(FBFloatVector phase, FBFloatVector incr)
{
  FBFloatVector y = 0.0f;
  FBFloatVector one = 1.0f;
  FBFloatVector zero = 0.0f;
  auto phaseLtIncrMask = xsimd::lt(phase, incr);
  auto phaseGte0Mask = xsimd::ge(phase, FBFloatVector(0.0f));
  auto phaseLt2IncrMask = xsimd::lt(phase, FBFloatVector(2.0f) * incr);
  auto phaseBetween0And2IncrMask = xsimd::bitwise_and(phaseGte0Mask, phaseLt2IncrMask);
  FBFloatVector phaseLtIncrMul = xsimd::select(phaseLtIncrMask, one, zero);
  FBFloatVector phaseBetween0And2IncrMul = xsimd::select(phaseBetween0And2IncrMask, one, zero);
  FBFloatVector x = phase / incr;
  FBFloatVector u = 2.0f - x;
  FBFloatVector u2 = u * u;
  u *= u2 * u2;
  y -= phaseBetween0And2IncrMul * u;
  FBFloatVector v = 1.0f - x;
  FBFloatVector v2 = v * v;
  v *= v2 * v2;
  y += 4.0f * phaseBetween0And2IncrMul * phaseLtIncrMul * v;
  return y * incr / 15.0f;
}

static FBFloatVector
GenerateTri(FBFloatVector phase, FBFloatVector incr)
{
  FBFloatVector v = 2.0f * xsimd::abs(2.0f * phase - 1.0f) - 1.0f;
  v += GenerateBLAMP(phase, incr);
  v += GenerateBLAMP(1.0f - phase, incr);
  phase += 0.5f;
  phase -= xsimd::floor(phase);
  v -= GenerateBLAMP(phase, incr);
  v -= GenerateBLAMP(1.0f - phase, incr);
  return v;
}

static FBFloatVector
GenerateDSF(
  FBFloatVector phase, FBFloatVector freq, FBFloatVector decay,
  FBFloatVector distFreq, FBFloatVector overtones)
{
  float const decayRange = 0.99f;
  float const scaleFactor = 0.975f;

  FBFloatVector n = overtones;
  FBFloatVector w = decay * decayRange;
  FBFloatVector wPowNp1 = xsimd::pow(w, n + 1.0f);
  FBFloatVector u = 2.0f * FBPi * phase;
  FBFloatVector v = 2.0f * FBPi * distFreq * phase / freq;
  FBFloatVector a = w * xsimd::sin(u + n * v) - xsimd::sin(u + (n + 1.0f) * v);
  FBFloatVector x = (w * xsimd::sin(v - u) + xsimd::sin(u)) + wPowNp1 * a;
  FBFloatVector y = 1.0f + w * w - 2.0f * w * xsimd::cos(v);
  FBFloatVector scale = (1.0f - wPowNp1) / (1.0f - w);
  return x * scaleFactor / (y * scale);
}

static FBFloatVector
GenerateDSFOvertones(
  FBFloatVector phase, FBFloatVector freq, FBFloatVector decay,
  FBFloatVector distFreq, FBFloatVector maxOvertones, int overtones_)
{
  FBFloatVector overtones = static_cast<float>(overtones_);
  overtones = xsimd::min(overtones, xsimd::floor(maxOvertones));
  return GenerateDSF(phase, freq, decay, distFreq, overtones);
}

static FBFloatVector
GenerateDSFBandwidth(
  FBFloatVector phase, FBFloatVector freq, FBFloatVector decay,
  FBFloatVector distFreq, FBFloatVector maxOvertones, float bandwidth)
{
  FBFloatVector overtones = 1.0f + xsimd::floor(bandwidth * (maxOvertones - 1.0f));
  overtones = xsimd::min(overtones, xsimd::floor(maxOvertones));
  return GenerateDSF(phase, freq, decay, distFreq, overtones);
}

void 
FFOsciProcessor::BeginVoice(FBModuleProcState const& state)
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
  _voiceState.dsfDistance = topo.NormalizedDiscreteFast(FFOsciParam::DSFDistance, params.block.dsfDistance[0].Voice()[voice]);
  _voiceState.dsfOvertones = topo.NormalizedDiscreteFast(FFOsciParam::DSFOvertones, params.block.dsfOvertones[0].Voice()[voice]);
  _voiceState.dsfBandwidthPlain = topo.NormalizedToIdentityFast(FFOsciParam::DSFBandwidth, params.block.dsfBandwidth[0].Voice()[voice]);
  _voiceState.unisonCount = topo.NormalizedDiscreteFast(FFOsciParam::UnisonCount, params.block.unisonCount[0].Voice()[voice]);
  _voiceState.unisonDetuneHQ = topo.NormalizedToBoolFast(FFOsciParam::UnisonDetuneHQ, params.block.unisonDetuneHQ[0].Voice()[voice]);
  _voiceState.unisonOffsetPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonOffset, params.block.unisonOffset[0].Voice()[voice]);
  _voiceState.unisonOffsetRandomPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonOffsetRandom, params.block.unisonOffsetRandom[0].Voice()[voice]);

  _prng = {};
  _phase = {};
  for (int i = 0; i < _voiceState.unisonCount; i++)
  {
    float offsetRandom = _voiceState.unisonOffsetRandomPlain;
    float unisonPhase = i * _voiceState.unisonOffsetPlain / _voiceState.unisonCount;
    _phases[i] = FBPhase(((1.0f - offsetRandom) + offsetRandom * _prng.Next()) * unisonPhase);
  }

  auto const& amParams = procState->param.voice.osciAM[0];
  auto const& amTopo = state.topo->static_.modules[(int)FFModuleType::OsciAM];
  for (int src = 0; src <= state.moduleSlot; src++)
  {
    auto const& srcOsciParams = procState->param.voice.osci[src];
    int slot = FFOsciModSourceAndTargetToSlot().at({ src, state.moduleSlot});
    _voiceState.amSourceOn[src] = amTopo.NormalizedToBoolFast(FFOsciAMParam::On, amParams.block.on[slot].Voice()[voice]);
    _voiceState.modSourceUnisonCount[src] = topo.NormalizedDiscreteFast(FFOsciParam::UnisonCount, srcOsciParams.block.unisonCount[0].Voice()[voice]);
  }
}

void 
FFOsciProcessor::ProcessBasicUnisonVoice(
  FBFixedFloatBlock& unisonAudioOut,
  FBFixedFloatBlock const& phase,
  FBFixedFloatBlock const& incr,
  FBFixedFloatBlock const& sinGainPlain,
  FBFixedFloatBlock const& sawGainPlain,
  FBFixedFloatBlock const& triGainPlain,
  FBFixedFloatBlock const& sqrGainPlain,
  FBFixedFloatBlock const& sqrPWPlain)
{
  unisonAudioOut.Fill(0.0f);
  if (_voiceState.basicSinOn)
    for (int v = 0; v < FBFixedFloatVectors; v++)
      unisonAudioOut[v] += GenerateSin(phase[v]) * sinGainPlain[v];
  if (_voiceState.basicSawOn)
    for (int v = 0; v < FBFixedFloatVectors; v++)
      unisonAudioOut[v] += GenerateSaw(phase[v], incr[v]) * sawGainPlain[v];
  if (_voiceState.basicTriOn)
    for (int v = 0; v < FBFixedFloatVectors; v++)
      unisonAudioOut[v] += GenerateTri(phase[v], incr[v]) * triGainPlain[v];
  if (_voiceState.basicSqrOn)
    for (int v = 0; v < FBFixedFloatVectors; v++)
      unisonAudioOut[v] += GenerateSqr(phase[v], incr[v], sqrPWPlain[v]) * sqrGainPlain[v];
}

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
void
FFOsciProcessor::ProcessDSFUnisonVoice(
  float sampleRate,
  FBFixedFloatBlock& unisonAudioOut,
  FBFixedFloatBlock const& phase,
  FBFixedFloatBlock const& freq,
  FBFixedFloatBlock const& decayPlain)
{
  FBFixedFloatBlock distFreq;
  FBFixedFloatBlock maxOvertones;
  distFreq.Transform([&](int v) { return static_cast<float>(_voiceState.dsfDistance) * freq[v]; });
  maxOvertones.Transform([&](int v) { return (sampleRate * 0.5f - freq[v]) / distFreq[v]; });

  if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
    unisonAudioOut.Transform([&](int v) { return GenerateDSFOvertones(
      phase[v], freq[v], decayPlain[v], distFreq[v], maxOvertones[v], _voiceState.dsfOvertones); });
  else
    unisonAudioOut.Transform([&](int v) { return GenerateDSFBandwidth(
      phase[v], freq[v], decayPlain[v], distFreq[v], maxOvertones[v], _voiceState.dsfBandwidthPlain); });
}

void 
FFOsciProcessor::ProcessTypeUnisonVoice(
  float sampleRate,
  FBFixedFloatBlock& unisonAudioOut,
  FBFixedFloatBlock const& phase,
  FBFixedFloatBlock const& freq,
  FBFixedFloatBlock const& incr,
  FBFixedFloatBlock const& basicSinGainPlain,
  FBFixedFloatBlock const& basicSawGainPlain,
  FBFixedFloatBlock const& basicTriGainPlain,
  FBFixedFloatBlock const& basicSqrGainPlain,
  FBFixedFloatBlock const& basicSqrPWPlain,
  FBFixedFloatBlock const& dsfDecayPlain)
{
  if (_voiceState.type == FFOsciType::Basic)
    ProcessBasicUnisonVoice(unisonAudioOut, phase, incr,
      basicSinGainPlain, basicSawGainPlain, 
      basicTriGainPlain, basicSqrGainPlain, basicSqrPWPlain);
  else if (_voiceState.type == FFOsciType::DSF)
    ProcessDSFUnisonVoice(sampleRate, unisonAudioOut, phase, freq, dsfDecayPlain);
  else
    assert(false);
}

void
FFOsciProcessor::ProcessUnisonVoice(
  FBModuleProcState const& state,
  int unisonVoice,
  FBFixedFloatBlock& unisonAudioOut,
  FBFixedFloatBlock const& unisonPos,
  FBFixedFloatBlock const& basePitch,
  FBFixedFloatBlock const& detunePlain,
  FBFixedFloatBlock const& basicSinGainPlain,
  FBFixedFloatBlock const& basicSawGainPlain,
  FBFixedFloatBlock const& basicTriGainPlain,
  FBFixedFloatBlock const& basicSqrGainPlain,
  FBFixedFloatBlock const& basicSqrPWPlain,
  FBFixedFloatBlock const& dsfDecayPlain)
{
  FBFixedFloatBlock incr;
  FBFixedFloatBlock freq;
  FBFixedFloatBlock phase;
  FBFixedFloatBlock pitch; 
  
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();

  for (int v = 0; v < FBFixedFloatVectors; v++)
  {
    pitch[v] = basePitch[v] + unisonPos[v] * detunePlain[v];
    if(_voiceState.unisonDetuneHQ || _voiceState.unisonCount == 1)
      freq[v] = FBPitchToFreqAccurate(pitch[v], sampleRate);
    else
      freq[v] = FBPitchToFreqFastAndInaccurate(pitch[v]);
    incr[v] = freq[v] / sampleRate;
    phase[v] = _phases[unisonVoice].Next(incr[v]);
  }

  ProcessTypeUnisonVoice(sampleRate, unisonAudioOut, phase, freq, incr,
    basicSinGainPlain, basicSawGainPlain, basicTriGainPlain, 
    basicSqrGainPlain, basicSqrPWPlain, dsfDecayPlain);

  auto* exchangeFromGUI = state.ExchangeFromGUIAs<FFExchangeState>();
  for (int src = 0; src <= state.moduleSlot; src++)
    if (_voiceState.amSourceOn[src] && _voiceState.modSourceUnisonCount[src] > unisonVoice)
    {
      FBFixedFloatBlock am;
      FBFixedFloatBlock mix;
      FBFixedFloatBlock ring;
      FBFixedFloatBlock modulated;
      FBFixedFloatBlock amModulated;
      FBFixedFloatBlock rmModulated;
      
      int slot = FFOsciModSourceAndTargetToSlot().at({ src, state.moduleSlot });
      if (exchangeFromGUI != nullptr)
      {
        mix.Fill(exchangeFromGUI->param.voice.osciAM[0].acc.mix[slot][voice]);
        ring.Fill(exchangeFromGUI->param.voice.osciAM[0].acc.ring[slot][voice]);
      } else 
      {
        mix.CopyFrom(procState->dsp.voice[voice].osciAM.outputMix[slot]);
        ring.CopyFrom(procState->dsp.voice[voice].osciAM.outputRing[slot]);
      }

      auto const& rm = procState->dsp.voice[voice].osci[src].unisonOutput[unisonVoice];
      am.Transform([&](int v) { return rm[v] * 0.5f + 0.5f; });
      amModulated.Transform([&](int v) { return unisonAudioOut[v] * am[v]; });
      rmModulated.Transform([&](int v) { return unisonAudioOut[v] * rm[v]; });
      modulated.Transform([&](int v) { return (1.0f - ring[v]) * amModulated[v] + ring[v] * rmModulated[v]; });
      unisonAudioOut.Transform([&](int v) { return (1.0f - mix[v]) * unisonAudioOut[v] + mix[v] * modulated[v]; });
    }
}

void 
FFOsciProcessor::ProcessUnison(
  FBModuleProcState& state,
  FBFixedFloatAudioBlock& audioOut,
  std::array<FBFixedFloatBlock, FFOsciUnisonMaxCount>& unisonAudioOut,
  FBFixedFloatBlock const& basePitch)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  FBFixedFloatBlock dsfDecayPlain;
  FBFixedFloatBlock basicSqrPWPlain;
  FBFixedFloatBlock basicSinGainPlain;
  FBFixedFloatBlock basicSawGainPlain;
  FBFixedFloatBlock basicTriGainPlain;
  FBFixedFloatBlock basicSqrGainPlain;

  auto const& dsfDecayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  auto const& detuneNorm = procParams.acc.unisonDetune[0].Voice()[voice];
  auto const& spreadNorm = procParams.acc.unisonSpread[0].Voice()[voice];
  auto const& basicSqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& basicSqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];
  auto const& basicSinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];

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
  else
    assert(false);

  FBFixedFloatBlock unisonPos;
  FBFixedFloatBlock detunePlain;
  if (_voiceState.unisonCount == 1)
  {
    unisonPos.Fill(0.0f);
    detunePlain.Fill(0.0f);
    ProcessUnisonVoice(
      state, 0, unisonAudioOut[0], unisonPos,
      basePitch, detunePlain, basicSinGainPlain, basicSawGainPlain, 
      basicTriGainPlain, basicSqrGainPlain, basicSqrPWPlain, dsfDecayPlain);
    for (int v = 0; v < FBFixedFloatVectors; v++)
    {
      audioOut[0][v] = unisonAudioOut[0][v];
      audioOut[1][v] = unisonAudioOut[0][v];
    }
  }
  else
  {
    audioOut.Fill(0.0f);
    FBFixedFloatBlock panning;
    FBFixedFloatBlock spreadPlain;
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonDetune, detuneNorm, detunePlain);
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonSpread, spreadNorm, spreadPlain);

    //float attenuate = 1.0f / std::sqrt(static_cast<float>(_voiceState.unisonCount));
    for (int i = 0; i < _voiceState.unisonCount; i++)
    {
      unisonPos.Fill(i / (_voiceState.unisonCount - 1.0f) - 0.5f);
      ProcessUnisonVoice(
        state, i, unisonAudioOut[i], unisonPos,
        basePitch, detunePlain, basicSinGainPlain, basicSawGainPlain,
        basicTriGainPlain, basicSqrGainPlain, basicSqrPWPlain, dsfDecayPlain);
      for (int v = 0; v < FBFixedFloatVectors; v++)
      {
        panning[v] = 0.5f + unisonPos[v] * spreadPlain[v];
        audioOut[0][v] += (1.0f - panning[v]) * unisonAudioOut[i][v];// *attenuate;
        audioOut[1][v] += panning[v] * unisonAudioOut[i][v];// *attenuate;
      }
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.unisonDetune[0][voice] = detuneNorm.Last();
  exchangeParams.acc.unisonSpread[0][voice] = spreadNorm.Last();
  exchangeParams.acc.dsfDecay[0][voice] = dsfDecayNorm.Last();
  exchangeParams.acc.basicSqrPW[0][voice] = basicSqrPWNorm.Last();
  exchangeParams.acc.basicSinGain[0][voice] = basicSinGainNorm.Last();
  exchangeParams.acc.basicSawGain[0][voice] = basicSawGainNorm.Last();
  exchangeParams.acc.basicTriGain[0][voice] = basicTriGainNorm.Last();
  exchangeParams.acc.basicSqrGain[0][voice] = basicSqrGainNorm.Last();
}

int
FFOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& output = procState->dsp.voice[voice].osci[state.moduleSlot].output;
  auto& unisonOutput = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutput;

  if (_voiceState.type == FFOsciType::Off)
  {
    output.Fill(0.0f);
    return 0;
  }

  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();

  FBFixedFloatBlock centPlain;
  auto const& centNorm = procParams.acc.cent[0].Voice()[voice];
  topo.NormalizedToLinearFast(FFOsciParam::Cent, centNorm, centPlain);

  FBFixedFloatBlock baseFreq;
  FBFixedFloatBlock baseIncr;
  FBFixedFloatBlock basePitch;
  float notePitch = _voiceState.key + _voiceState.note - 60.0f;
  basePitch.Transform([&](int v) { return notePitch + centPlain[v]; });
  baseFreq.Transform([&](int v) { return FBPitchToFreqAccurate(basePitch[v], state.input->sampleRate); });
  baseIncr.Transform([&](int v) { return baseFreq[v] / state.input->sampleRate; });
  _phase.Next(baseIncr);

  ProcessUnison(state, output, unisonOutput, basePitch);

  FBFixedFloatBlock gainPlain;
  FBFixedFloatBlock gLFOToGainPlain;
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& gLFOToGainNorm = procParams.acc.gLFOToGain[0].Voice()[voice];
  topo.NormalizedToIdentityFast(FFOsciParam::Gain, gainNorm, gainPlain);
  topo.NormalizedToIdentityFast(FFOsciParam::GLFOToGain, gLFOToGainNorm, gLFOToGainPlain);

  // TODO this might prove difficult, lets see how it fares with the matrices
  FBFixedFloatBlock gLFO;
  auto* exchangeFromGUI = state.ExchangeFromGUIAs<FFExchangeState>();
  if (exchangeFromGUI != nullptr)
    gLFO.Fill(exchangeFromGUI->global.gLFO[0].lastOutput);
  else
    gLFO.CopyFrom(procState->dsp.global.gLFO[0].output);

  FBFixedFloatBlock gainWithGLFOBlock;
  gainWithGLFOBlock.Transform([&](int v) { return ((1.0f - gLFOToGainPlain[v]) + gLFOToGainPlain[v] * gLFO[v]) * gainPlain[v]; });
  output.Transform([&](int ch, int v) { return output[ch][v] * gainWithGLFOBlock[v]; });

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreq.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.cent[0][voice] = centNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainWithGLFOBlock.Last();
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGainNorm.Last();
  return FBFixedBlockSamples;
}