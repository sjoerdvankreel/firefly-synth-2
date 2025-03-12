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
  _voiceState.unisonCount = topo.NormalizedDiscreteFast(FFOsciParam::UnisonCount, params.block.unisonCount[0].Voice()[voice]);
  _voiceState.unisonOffset = topo.NormalizedToIdentityFast(FFOsciParam::UnisonOffset, params.block.unisonOffset[0].Voice()[voice]);
  _voiceState.basicSinOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSinOn, params.block.basicSinOn[0].Voice()[voice]);
  _voiceState.basicSawOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSawOn, params.block.basicSawOn[0].Voice()[voice]);
  _voiceState.basicTriOn = topo.NormalizedToBoolFast(FFOsciParam::BasicTriOn, params.block.basicTriOn[0].Voice()[voice]);
  _voiceState.basicSqrOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSqrOn, params.block.basicSqrOn[0].Voice()[voice]);
  _voiceState.dsfMode = topo.NormalizedToListFast<FFOsciDSFMode>(FFOsciParam::DSFMode, params.block.dsfMode[0].Voice()[voice]);
  _voiceState.dsfDistance = topo.NormalizedDiscreteFast(FFOsciParam::DSFDistance, params.block.dsfDistance[0].Voice()[voice]);
  _voiceState.dsfOvertones = topo.NormalizedDiscreteFast(FFOsciParam::DSFOvertones, params.block.dsfOvertones[0].Voice()[voice]);
  _voiceState.dsfBandwidth = topo.NormalizedToIdentityFast(FFOsciParam::DSFBandwidth, params.block.dsfBandwidth[0].Voice()[voice]);

  _phase = {};
  for (int i = 0; i < _voiceState.unisonCount; i++)
    _phases[i] = FBPhase(i * _voiceState.unisonOffset / _voiceState.unisonCount);
}

void 
FFOsciProcessor::ProcessBasic(
  FBModuleProcState& state, 
  FBFixedFloatBlock const& phase,
  FBFixedFloatBlock const& incr,
  FBFixedFloatBlock& audioOut)
{
  FBFixedFloatBlock audio;
  FBFixedFloatBlock pwPlain;
  FBFixedFloatBlock gainPlain;

  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& sqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& sinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& sawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& triGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];
  auto const& sqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];

  audioOut.Fill(0.0f);
  if (_voiceState.basicSinOn)
  {
    topo.NormalizedToLinearFast(FFOsciParam::BasicSinGain, sinGainNorm, gainPlain);
    audio.Transform([&](int v) { return GenerateSin(phase[v]) * gainPlain[v]; });
    audioOut.Add(audio);
  }
  if (_voiceState.basicSawOn)
  {
    topo.NormalizedToLinearFast(FFOsciParam::BasicSawGain, sawGainNorm, gainPlain);
    audio.Transform([&](int v) { return GenerateSaw(phase[v], incr[v]) * gainPlain[v]; });
    audioOut.Add(audio);
  }
  if (_voiceState.basicTriOn)
  {
    topo.NormalizedToLinearFast(FFOsciParam::BasicTriGain, triGainNorm, gainPlain);
    audio.Transform([&](int v) { return GenerateTri(phase[v], incr[v]) * gainPlain[v]; });
    audioOut.Add(audio);
  }
  if (_voiceState.basicSqrOn)
  {
    topo.NormalizedToLinearFast(FFOsciParam::BasicSqrGain, sqrGainNorm, gainPlain);
    topo.NormalizedToIdentityFast(FFOsciParam::BasicSqrPW, sqrPWNorm, pwPlain);
    audio.Transform([&](int v) { return GenerateSqr(phase[v], incr[v], pwPlain[v]) * gainPlain[v]; });
    audioOut.Add(audio);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.basicSqrPW[0][voice] = sqrPWNorm.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicSinGain[0][voice] = sinGainNorm.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicSawGain[0][voice] = sawGainNorm.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicTriGain[0][voice] = triGainNorm.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.basicSqrGain[0][voice] = sqrGainNorm.CV().data[FBFixedBlockSamples - 1];
}

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
void
FFOsciProcessor::ProcessDSF(
  FBModuleProcState& state,
  FBFixedFloatBlock const& phase,
  FBFixedFloatBlock const& freq,
  FBFixedFloatBlock& audioOut)
{
  FBFixedFloatBlock distFreq;
  FBFixedFloatBlock decayPlain;
  FBFixedFloatBlock maxOvertones;

  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  
  auto const& decayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  topo.NormalizedToIdentityFast(FFOsciParam::DSFDecay, decayNorm, decayPlain);
  distFreq.Transform([&](int v) { return static_cast<float>(_voiceState.dsfDistance) * freq[v]; });
  maxOvertones.Transform([&](int v) { return (sampleRate * 0.5f - freq[v]) / distFreq[v]; });

  if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
    audioOut.Transform([&](int v) { return GenerateDSFOvertones(
      phase[v], freq[v], decayPlain[v], distFreq[v], maxOvertones[v], _voiceState.dsfOvertones); });
  else
    audioOut.Transform([&](int v) { return GenerateDSFBandwidth(
      phase[v], freq[v], decayPlain[v], distFreq[v], maxOvertones[v], _voiceState.dsfBandwidth); });

  // not for all voices
  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.dsfDecay[0][voice] = decayNorm.CV().data[FBFixedBlockSamples - 1];
}

void 
FFOsciProcessor::ProcessType(
  FBModuleProcState& state,
  FBFixedFloatBlock const& phase,
  FBFixedFloatBlock const& freq,
  FBFixedFloatBlock const& incr,
  FBFixedFloatBlock& audioOut)
{
  if (_voiceState.type == FFOsciType::Basic)
    ProcessBasic(state, phase, incr, audioOut);
  else if (_voiceState.type == FFOsciType::DSF)
    ProcessDSF(state, phase, freq, audioOut);
  else
    assert(false);
}

void
FFOsciProcessor::ProcessUnisonVoice(
  FBModuleProcState& state,
  FBFixedFloatBlock const& basePitch,
  FBFixedFloatBlock const& detunePlain,
  FBFixedFloatBlock& audioOut,
  int unisonVoice, float pos)
{
  FBFixedFloatBlock incr;
  FBFixedFloatBlock freq;
  FBFixedFloatBlock phase;
  FBFixedFloatBlock pitch;
  
  pitch.Transform([&](int v) { return basePitch[v] + (pos - 0.5f) * detunePlain[v]; });
  freq.Transform([&](int v) { return FBPitchToFreq(pitch[v], state.input->sampleRate); });
  incr.Transform([&](int v) { return freq[v] / state.input->sampleRate; });
  phase.Transform([&](int v) { return _phases[unisonVoice].Next(incr[v]); });
  ProcessType(state, phase, freq, incr, audioOut);
}

void 
FFOsciProcessor::ProcessUnison(
  FBModuleProcState& state,
  FBFixedFloatBlock const& basePitch,
  FBFixedFloatAudioBlock& audioOut)
{
  FBFixedFloatBlock osciOut;
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  FBFixedFloatBlock detunePlain;
  if (_voiceState.unisonCount == 1)
  {
    detunePlain.Fill(0.0f);
    ProcessUnisonVoice(state, basePitch, detunePlain, osciOut, 0, 0.5f);
    audioOut.Transform([&](int ch, int v) { return osciOut[v]; });
    return;
  }

  audioOut.Fill(0.0f);
  FBFixedFloatBlock panning;
  FBFixedFloatBlock spreadPlain;
  auto const& detuneNorm = procParams.acc.unisonDetune[0].Voice()[voice];
  auto const& spreadNorm = procParams.acc.unisonSpread[0].Voice()[voice];
  topo.NormalizedToIdentityFast(FFOsciParam::UnisonDetune, detuneNorm, detunePlain);
  topo.NormalizedToIdentityFast(FFOsciParam::UnisonSpread, spreadNorm, spreadPlain);

  float attenuate = 1.0f / std::sqrt(static_cast<float>(_voiceState.unisonCount));
  for (int i = 0; i < _voiceState.unisonCount; i++)
  {
    float pos = i / (_voiceState.unisonCount - 1.0f);
    ProcessUnisonVoice(state, basePitch, detunePlain, osciOut, i, pos);
    panning.Transform([&](int v) { return 0.5f + (pos - 0.5f) * spreadPlain[v]; });
    audioOut[0].Transform([&](int v) { return (1.0f - panning[v]) * osciOut[v] * attenuate; });
    audioOut[1].Transform([&](int v) { return panning[v] * osciOut[v] * attenuate; });
  }
}

int
FFOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& output = procState->dsp.voice[voice].osci[state.moduleSlot].output;

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
  baseFreq.Transform([&](int v) { return FBPitchToFreq(basePitch[v], state.input->sampleRate); });
  baseIncr.Transform([&](int v) { return baseFreq[v] / state.input->sampleRate; });
  _phase.Next(baseIncr);

  ProcessUnison(state, basePitch, output);

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