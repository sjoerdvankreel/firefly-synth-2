#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

static FBFloatVector
GenerateSin(FBFloatVector phase)
{
  return xsimd::sin(phase * FBTwoPi);
}

static FBFloatVector
GenerateSaw(FBFloatVector phase, FBFloatVector incr)
{ 
  // TODO fast paul saw
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
  auto hiMask = xsimd::bitwise_andnot(loMask, xsimd::ge(phase, 1.0f - incr));
  FBFloatVector loMul = xsimd::select(loMask, zero, one);
  FBFloatVector hiMul = xsimd::select(hiMask, zero, one);
  result -= loMul * ((2.0f - blepLo) * blepLo - 1.0f);
  result -= hiMul * ((blepHi + 2.0f) * blepHi + 1.0f);
  return result;
}

void
FFOsciProcessor::Process(FFModuleProcState const& state, int voice)
{
  float key = static_cast<float>(state.voice->event.note.key);
  auto const& glfo = state.proc->dsp.global.glfo[0].output;
  auto& output = state.proc->dsp.voice[voice].osci[state.moduleSlot].output;

  auto const& topo = state.topo->modules[FFModuleOsci];
  auto const& params = state.proc->param.voice.osci[state.moduleSlot];
  auto const& gain = params.acc.gain[0].Voice()[voice].CV();
  auto const& glfoToGain = params.acc.glfoToGain[0].Voice()[voice].CV();
  bool on = topo.params[FFOsciBlockOn].NormalizedToBool(params.block.on[0].Voice()[voice]);
  int type = topo.params[FFOsciBlockType].NormalizedToDiscrete(params.block.type[0].Voice()[voice]);
  int note = topo.params[FFOsciBlockNote].NormalizedToDiscrete(params.block.note[0].Voice()[voice]);

  if (!on)
  {
    output.Clear();
    return;
  }

  FBFixedVectorBlock incr;
  FBFixedVectorBlock phase;
  incr.Transform([&](int v) {
    auto cent = params.acc.cent[0].Voice()[voice].CV(v);
    auto centPlain = topo.params[FFOsciAccCent].NormalizedToPlainLinear(cent);
    auto pitch = key + note - 60.0f + centPlain;
    auto freq = FBPitchToFreq(pitch, state.sampleRate);
    return freq / state.sampleRate; });
  phase.Transform([&](int v) { return _phase.Next(incr[v]); });

  switch (type)
  {
  case FFOsciTypeSine: output.Transform([&](int ch, int v) { 
    return GenerateSin(output[ch][v]); }); break;
  case FFOsciTypeSaw: output.Transform([&](int ch, int v) {
    return GenerateSaw(output[ch][v], incr[v]); }); break;
  default: assert(false); break;
  }
  output.Transform([&](int ch, int v) { 
    return (1.0f - glfoToGain[v]) * output[ch][v] + output[ch][v] * glfoToGain[v] * glfo[v]; });
  output.Transform([&](int ch, int v) { 
    return output[ch][v] * gain[v]; });
}