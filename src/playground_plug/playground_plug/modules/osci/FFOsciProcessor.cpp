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
  return (phase * FBTwoPi).Sin();
}

// https://www.kvraudio.com/forum/viewtopic.php?t=375517
static inline float
generate_blep(float phase, float inc)
{
  float b;
  if (phase < inc) return b = phase / inc, (2.0f - b) * b - 1.0f;
  if (phase >= 1.0f - inc) return b = (phase - 1.0f) / inc, (b + 2.0f) * b + 1.0f;
  return 0.0f;
}

static inline float
generate_saw(float phase, float inc)
{
  float saw = phase * 2 - 1;
  return saw - generate_blep(phase, inc);
}

static FBFloatVector
GenerateSaw(FBFloatVector phase, FBFloatVector incr)
{ 
  FBFloatVector y;
  for (int i = 0; i < FBVectorFloatCount; i++)
  {
    y[i] = generate_saw(phase[i], incr[i]);
  }
  return y;
  
  // TODO the fast version
  // https://github.com/surge-synthesizer/clap-saw-demo
  // float phaseSteps[3];
  // for (int q = -2; q <= 0; ++q) {
  //   float ph = phase + q * incr;
  //   ph -= std::floor(ph);
  //   ph = ph * 2.0f - 1.0f;
  //   phaseSteps[q + 2] = (ph * ph - 1.0f) * ph / 6.0f; }
  // float scale = 0.25f * (1.0f / incr) * (1.0f / incr);
  // y = scale * (phaseSteps[0] + phaseSteps[2] - 2.0f * phaseSteps[1]);
#if 0
  FBFloatVector result;
  FBFloatVector phaseSteps[3];
  FBFloatVector incrReciprocal = incr.Reciprocal();
  for (int q = -2; q <= 0; ++q)
  {
    FBFloatVector ph = phase + static_cast<float>(q) * incr;
    ph = (ph - ph.Floor()).Bipolar();
    phaseSteps[q + 2] = (ph * ph - 1.0f) * ph / 6.0f;
  }
  FBFloatVector scale = 0.25f * incrReciprocal * incrReciprocal;
  return scale * (phaseSteps[0] + phaseSteps[2] - 2.0f * phaseSteps[1]);
#endif
}

void
FFOsciProcessor::Process(FFModuleProcState const& state, int voice)
{
  int key = state.voice->event.note.key;
  float freq = FBPitchToFreq(static_cast<float>(key));
  auto const& glfo = state.proc->dsp.global.glfo[0].output;
  auto& output = state.proc->dsp.voice[voice].osci[state.moduleSlot].output;

  auto const& topo = state.topo->modules[FFModuleOsci];
  auto const& params = state.proc->param.voice.osci[state.moduleSlot];
  auto const& gain = params.acc.gain[0].Voice()[voice].CV();
  auto const& cent = params.acc.cent[0].Voice()[voice].CV();
  auto const& glfoToGain = params.acc.glfoToGain[0].Voice()[voice].CV();
  bool on = topo.params[FFOsciBlockOn].NormalizedToBool(params.block.on[0].Voice()[voice]);
  int type = topo.params[FFOsciBlockType].NormalizedToDiscrete(params.block.type[0].Voice()[voice]);
  int note = topo.params[FFOsciBlockNote].NormalizedToDiscrete(params.block.note[0].Voice()[voice]);

  if (!on)
  {
    output.Clear();
    return;
  }

  float incr = freq / state.sampleRate; // TODO this should be dynamic
  for (int s = 0; s < FBFixedBlockSamples; s++)
    output.Samples(s, _phase.Next(state.sampleRate, freq));
  switch (type)
  {
  case FFOsciTypeSine: output.Transform([&](int ch, int v) { 
    return GenerateSin(output[ch][v]); }); break;
  case FFOsciTypeSaw: output.Transform([&](int ch, int v) {
    return GenerateSaw(output[ch][v], FBFloatVector(incr)); }); break;
  default: assert(false); break;
  }
  output.Transform([&](int ch, int v) { 
    return (1.0f - glfoToGain[v]) * output[ch][v] + output[ch][v] * glfoToGain[v] * glfo[v]; });
  output.Transform([&](int ch, int v) { 
    return output[ch][v] * gain[v]; });
}