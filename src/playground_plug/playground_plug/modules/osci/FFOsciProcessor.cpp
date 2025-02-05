#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

static FBFloatVector
GenerateSin(FBFloatVector phase)
{
  return xsimd::sin(phase * FBTwoPi);
}

static FBFloatVector
GenerateSaw(FBFloatVector phase, FBFloatVector incr)
{ 
  // https://www.kvraudio.com/forum/viewtopic.php?t=375517
  // y = phase * 2 - 1
  // if (phase < inc) y -= b = phase / inc, (2.0f - b) * b - 1.0f
  // else if (phase >= 1.0f - inc) y -= b = (phase - 1.0f) / inc, (b + 2.0f) * b + 1.0f
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
GeneratePulse(FBFloatVector phase, FBFloatVector incr, FBFloatVector pw)
{
  FBFloatVector minPW = 0.05f;
  FBFloatVector realPW = (minPW + (1.0f - minPW) * pw) * 0.5f;
  FBFloatVector phase2 = phase + realPW;
  phase2 -= xsimd::floor(phase2);
  return (GenerateSaw(phase, incr) - GenerateSaw(phase2, incr)) * 0.5f;
}

void 
FFOsciProcessor::BeginVoice(FBModuleProcState const& state)
{
  _phase = {};
  int voice = state.voice->slot;
  auto* procState = state.ProcState<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  _voiceState.key = (float)state.voice->event.note.key;
  _voiceState.on = topo.params[(int)FFOsciParam::On].boolean.NormalizedToPlain(
    params.block.on[0].Voice()[voice]);
  _voiceState.note = topo.params[(int)FFOsciParam::Note].discrete.NormalizedToPlain(
    params.block.note[0].Voice()[voice]);
  _voiceState.type = (FFOsciType)topo.params[(int)FFOsciParam::Type].list.NormalizedToPlain(
    params.block.type[0].Voice()[voice]);
}

void
FFOsciProcessor::Process(FBModuleProcState const& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcState<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& pw = procParams.acc.pw[0].Voice()[voice];
  auto const& cent = procParams.acc.cent[0].Voice()[voice];
  auto const& gain1 = procParams.acc.gain[0].Voice()[voice];
  auto const& gain2 = procParams.acc.gain[1].Voice()[voice];
  auto const& gLFOToGain = procParams.acc.gLFOToGain[0].Voice()[voice];

  auto const& gLFO = procState->dsp.global.gLFO[0].output;
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& output = procState->dsp.voice[voice].osci[state.moduleSlot].output;

  if (!_voiceState.on)
  {
    output.Fill(0.0f);
    return;
  }

  FBFixedFloatBlock incr;
  FBFixedFloatBlock mono;
  FBFixedFloatBlock phase;
  incr.Transform([&](int v) {
    auto centPlain = topo.params[(int)FFOsciParam::Cent].linear.NormalizedToPlain(cent.CV(v));
    auto pitch = _voiceState.key + _voiceState.note - 60.0f + centPlain;
    auto freq = FBPitchToFreq(pitch, state.sampleRate);
    return freq / state.sampleRate; });
  phase.Transform([&](int v) { return _phase.Next(incr[v]); });

  switch (_voiceState.type)
  {
  case FFOsciType::Sine: mono.Transform([&](int v) {
    return GenerateSin(phase[v]); }); break;
  case FFOsciType::Saw: mono.Transform([&](int v) {
    return GenerateSaw(phase[v], incr[v]); }); break;
  case FFOsciType::Pulse: mono.Transform([&](int v) {
    return GeneratePulse(phase[v], incr[v], pw.CV(v)); }); break;
  default: 
    assert(false); break;
  }

  FBFixedFloatBlock gain1WithGLFOBlock;
  gain1WithGLFOBlock.Transform([&](int v) {
    auto gLFOToGainBlock = gLFOToGain.CV(v);
    return (1.0f - gLFOToGainBlock) * gain1.CV(v) + gLFOToGainBlock * gLFO[v] * gain1.CV(v); });

  mono.Transform([&](int v) {
    return gain1WithGLFOBlock[v] * gain2.CV(v) * mono[v]; });
  output.Transform([&](int ch, int v) { return mono[v]; });

  auto* exchangeState = state.ExchangeState<FFExchangeState>();
  if (exchangeState == nullptr)
    return; 
  exchangeState->voice[voice].osci[state.moduleSlot].active = true;
  auto& exchangeParams = exchangeState->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.gain[0][voice] = gain1WithGLFOBlock.Last();
  exchangeParams.acc.pw[0][voice] = pw.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.cent[0][voice] = cent.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.gain[1][voice] = gain2.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGain.CV().data[FBFixedBlockSamples - 1];
}