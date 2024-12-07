#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFModuleProcState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>

void
FFOsciProcessor::Process(FFModuleProcState const& state, int voice)
{
  auto const& topo = state.topo->modules[FFModuleOsci];
  auto const& params = state.proc->param.voice.osci[state.moduleSlot];
  auto& output = state.proc->dsp.voice[voice].osci[state.moduleSlot].output;
  bool on = topo.params[FFOsciBlockOn].NormalizedToBool(block.on[0].voice[voice]);

  if (!on)
  {
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  // TODO simd lib
  int key = state.voice->event.note.key;
  float freq = FBPitchToFreq(static_cast<float>(key));
  auto const& gain = acc.gain[0].proc[]
  for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> *_phase);
    output[0][s] = sample * gain[s];
    output[1][s] = sample * gain[s];
    // todo continuous pitch
    _phase += freq / state.sampleRate;
    _phase -= std::floor(_phase);
  }
}