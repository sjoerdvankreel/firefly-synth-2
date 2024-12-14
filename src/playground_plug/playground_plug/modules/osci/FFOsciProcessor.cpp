#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFModuleProcState.hpp> // TODO
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

struct FFOsciBlock
{
  FFModuleProcState const& state;
  FBFixedAudioBlock& output;
  bool on;
  int type;
  FBFixedVectorBlock const& gain;
  FBFixedVectorBlock const& pitch;
  FBFixedVectorBlock const& glfoToGain;
};

void
FFOsciProcessor::Generate(FFOsciBlock& block)
{
  int key = block.state.voice->event.note.key;
  float freq = FBPitchToFreq(static_cast<float>(key));
  auto const& glfo = block.state.proc->dsp.global.glfo[0].output;

  for (int s = 0; s < FBFixedBlockSamples; s++)
    block.output.Samples(s, _phase.Next(block.state.sampleRate, freq));
  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedBlockVectors; v++)
    {
      block.output[ch][v] = (block.output[ch][v] * FBFloatVector::TwoPi()).Sin();
      block.output[ch][v] = (FBFloatVector::One() - block.glfoToGain[v]) * block.output[ch][v] + block.output[ch][v] * block.glfoToGain[v] * glfo[v];
      block.output[ch][v] *= block.gain[v];
    }
}

void
FFOsciProcessor::Process(FFModuleProcState const& state, int voice)
{
  auto const& topo = state.topo->modules[FFModuleOsci];
  auto const& params = state.proc->param.voice.osci[state.moduleSlot];
  FFOsciBlock block = {
    state,
    state.proc->dsp.voice[voice].osci[state.moduleSlot].output,
    topo.params[FFOsciBlockOn].NormalizedToBool(params.block.on[0].Voice()[voice]),
    topo.params[FFOsciBlockType].NormalizedToBool(params.block.type[0].Voice()[voice]),
    params.acc.gain[0].Voice()[voice].CV(),
    params.acc.pitch[0].Voice()[voice].CV(),
    params.acc.glfoToGain[0].Voice()[voice].CV() };

  if (block.on)
    Generate(block);
  else
    block.output.Clear();
}