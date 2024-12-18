#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

void
FFOsciProcessor::GenerateSine(FBFixedAudioBlock& output)
{
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedBlockVectors; v++)
      output[ch][v] = (output[ch][v] * FBFloatVector::TwoPi()).Sin();
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
  case FFOsciTypeSaw: output.Transform([&](int ch, int v) { return (output[ch][v] * FBFloatVector::TwoPi()).Sin(); }); break;
  case FFOsciTypeSine: output.Transform([&](int ch, int v) { return (output[ch][v] * FBFloatVector::TwoPi()).Sin(); }); break;
  default: assert(false); break;
  }
  output.Transform([&](int ch, int v) { return (FBFloatVector::One() - glfoToGain[v]) * output[ch][v] + output[ch][v] * glfoToGain[v] * glfo[v]; });
  output.Transform([&](int ch, int v) { return output[ch][v] * gain[v]; });
}