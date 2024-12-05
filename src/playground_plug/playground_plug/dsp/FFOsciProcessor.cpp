#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFModuleProcState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>

void
FFOsciProcessor::Process(FFModuleProcState const& state, int voice)
{
  auto const& params = state.proc->param.osci[voice];
  auto const& topo = state.topo->modules[FFModuleOsci];
  auto& output = state.proc->dsp.voices[voice].osci[voice].output;

  bool on = topo.params[FFOsciBlockOn].NormalizedToBool(params.block.on[state.moduleSlot]);
  if (!on)
  {
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  // TODO simd lib
  auto const& gain = params.acc.gain[0].smoothedCV;
  for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> *_phase);
    output[0][s] = sample * gain[s];
    output[1][s] = sample * gain[s];
    _phase += 440.0f / state.sampleRate; // todo pitch
    _phase -= std::floor(_phase);
  }
}