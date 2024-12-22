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
GenerateSaw(FBFloatVector phase, FBFloatVector incr)
{ 
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

  FBFixedVectorBlock freq;
  FBFixedVectorBlock incr;
  FBFixedVectorBlock pitch;
  pitch.Transform([&](int v) { return key + note - 60.0f + cent[v]; });
  freq.Transform([&](int v) { return FBPitchToFreq(pitch[v]); });
  incr.Transform([&](int v) { return freq[v] / state.sampleRate; });
  for (int s = 0; s < FBFixedBlockSamples; s++)
    output.Samples(s, _phase.Next(state.sampleRate, freq.Sample(s)));
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