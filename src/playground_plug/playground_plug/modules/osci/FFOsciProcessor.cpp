#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

static FBFloatVector
GenerateSine(FBFloatVector phase)
{
  return (phase * FBFloatVector::TwoPi()).Sin();
}

static FBFloatVector
GenerateBLEPSaw(FBFloatVector phase)
{
  return phase; // TODO
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
  auto const& glfoToGain = params.acc.glfoToGain[0].Voice()[voice].CV();
  bool on = topo.params[FFOsciBlockOn].NormalizedToBool(params.block.on[0].Voice()[voice]);
  bool type = topo.params[FFOsciBlockType].NormalizedToDiscrete(params.block.type[0].Voice()[voice]);

  if (!on)
  {
    output.Clear();
    return;
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
    output.Samples(s, _phase.Next(state.sampleRate, freq));
  switch (type)
  {
  case FFOsciTypeSine: output.Transform([&](int ch, int v) { 
    return GenerateSine(output[ch][v]); }); break;
  case FFOsciTypeBLEPSaw: output.Transform([&](int ch, int v) {
    return GenerateBLEPSaw(output[ch][v]); }); break;
  default: assert(false); break;
  }
  output.Transform([&](int ch, int v) { 
    return (1.0f - glfoToGain[v]) * output[ch][v] + output[ch][v] * glfoToGain[v] * glfo[v]; });
  output.Transform([&](int ch, int v) { 
    return output[ch][v] * gain[v]; });
}