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

static FBFloatVector
GenerateBLEPSaw(FBFloatVector phase, FBFloatVector incr)
{
  FBFloatVector result = phase * 2.0f - 1.0f;
  FBFloatVector phaseLtIncr = phase < incr;
  FBFloatVector phaseGteOneMinusIncr = phase >= 1.0f - incr;
  phaseGteOneMinusIncr *= 1.0f - phaseLtIncr;
  FBFloatVector blepLow = phase / incr;
  result -= phaseLtIncr * ((2.0f - blepLow) * blepLow - 1.0f);
  FBFloatVector blepHi = (phase - 1.0f) / incr;
  result -= phaseGteOneMinusIncr * ((blepHi + 2.0f) * blepHi + 1.0f);

  FBFloatVector res2 = result;
  for (int s = 0; s < FBVectorFloatCount; s++) // TODO simd conditionals
    if (phase[s] < incr[s])
    {
      float b = phase[s] / incr[s];
      res2[s] -= (2.0f - b) * b - 1.0f;
    }
    else if (phase[s] >= 1.0f - incr[s])
    {
      float b = (phase[s] - 1.0f) / incr[s];
      res2[s] -= (b + 2.0f) * b + 1.0f;
    }

  for(int i = 0; i < FBVectorFloatCount; i++)
    if (result[i] != res2[i])
    {
      int x = 0;
      x++;
    }

  return result;
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

  float incr = freq / state.sampleRate; // TODO this should be dynamic
  for (int s = 0; s < FBFixedBlockSamples; s++)
    output.Samples(s, _phase.Next(state.sampleRate, freq));
  switch (type)
  {
  case FFOsciTypeSine: output.Transform([&](int ch, int v) { 
    return GenerateSin(output[ch][v]); }); break;
  case FFOsciTypeBLEPSaw: output.Transform([&](int ch, int v) {
    return GenerateBLEPSaw(output[ch][v], FBFloatVector(incr)); }); break;
  default: assert(false); break;
  }
  output.Transform([&](int ch, int v) { 
    return (1.0f - glfoToGain[v]) * output[ch][v] + output[ch][v] * glfoToGain[v] * glfo[v]; });
  output.Transform([&](int ch, int v) { 
    return output[ch][v] * gain[v]; });
}