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
GenerateSurgeSaw(FBFloatVector phase, FBFloatVector incr)
{
  // stolen from clap-saw-demo
  FBFloatVector result;
  for (int v = 0; v < FBVectorFloatCount; v++)
  {
    /*
         * Use a cubic integrated saw and second derive it at
         * each point. This is basically the math I worked
         * out for the surge modern oscillator. The cubic function
         * which gives a clean saw is phase^3 / 6 - phase / 6.
         * Evaluate it at 3 points and then differentiate it like
         * we do in Surge Modern. The waveform is the same both
         * channels.
         */
    float phaseSteps[3];
    for (int q = -2; q <= 0; ++q)
    {
      float ph = phase[v] + q * incr[v];

      // Bind phase to 0...1. Lots of ways to do this
      ph = ph - floor(ph);

      // Our calculation assumes phase in -1,1 and this phase is
      // in 0 1 so
      ph = ph * 2.0f - 1.0f;
      phaseSteps[q + 2] = (ph * ph - 1) * ph / 6.0f;
    }
    // the 0.25 here is because of the phase rescaling again
    float saw = (phaseSteps[0] + phaseSteps[2] - 2.0f * phaseSteps[1]) * 0.25f * (1.0f / incr[v]) * (1.0f / incr[v]);
    result[v] = saw;
  }
  return result;
}

static FBFloatVector
GenerateBLEPSaw(FBFloatVector phase, FBFloatVector incr)
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
  FBFloatVector loMask = FBFloatVectorCmpLt(phase, incr);
  FBFloatVector hiMask = FBFloatVectorAndNot(loMask, FBFloatVectorCmpGe(phase, 1.0f - incr));
  FBFloatVector loMul = FBFloatVectorBlend(zero, one, loMask);
  FBFloatVector hiMul = FBFloatVectorBlend(zero, one, hiMask);
  result -= loMul * ((2.0f - blepLo) * blepLo - 1.0f);
  result -= hiMul * ((blepHi + 2.0f) * blepHi + 1.0f);
  return result;
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
  int type = topo.params[FFOsciBlockType].NormalizedToDiscrete(params.block.type[0].Voice()[voice]);

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
  case FFOsciTypeSurgeSaw: output.Transform([&](int ch, int v) {
    return GenerateSurgeSaw(output[ch][v], FBFloatVector(incr)); }); break;
  default: assert(false); break;
  }
  output.Transform([&](int ch, int v) { 
    return (1.0f - glfoToGain[v]) * output[ch][v] + output[ch][v] * glfoToGain[v] * glfo[v]; });
  output.Transform([&](int ch, int v) { 
    return output[ch][v] * gain[v]; });
}