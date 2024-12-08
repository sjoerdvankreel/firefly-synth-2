#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFModuleProcState.hpp>
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
  FBFixedCVBlock const& gain;
  FBFixedCVBlock const& pitch;
  FBFixedCVBlock const& glfoToGain;
};

void
FFOsciProcessor::ApplyGLFOToGain(FFOsciBlock& block)
{
  auto const& glfo = block.state.proc->dsp.global.glfo[0].output;
  block.output.InPlaceMultiplyByOneMinus(block.glfoToGain);
  block.output.InPlaceFMA(glfo, block.glfoToGain);
}

void
FFOsciProcessor::Generate(FFOsciBlock& block)
{
  int key = block.state.voice->event.note.key;
  float freq = FBPitchToFreq(static_cast<float>(key));
  for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
  {
    float sample = FBPhaseToSine(_phase.Next(block.state.sampleRate, freq));
    block.output[0][s] = sample;
    block.output[1][s] = sample;
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

  if (!block.on)
  {
    block.output.Fill(0, block.output.Count(), 0.0f);
    return;
  }

  Generate(block);
  block.output.InPlaceMultiplyBy(block.gain);
  ApplyGLFOToGain(block);
}