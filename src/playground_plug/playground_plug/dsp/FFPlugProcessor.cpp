#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/dsp/FFPlugProcessor.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <cmath>
#include <numbers>

FFPlugProcessor::
FFPlugProcessor(
FBRuntimeTopo const& topo, float sampleRate) :
_topo(topo.static_),
_sampleRate(sampleRate) {}

void 
FFPlugProcessor::ProcessPlug(
  FBFixedAudioBlock const& input, FBFixedAudioBlock& output)
{
  auto const& accState = _state.osci[0].acc;
  auto const& blockState = _state.osci[0].block;
  auto const& accTopo = _topo.modules[FFModuleOsci].acc;
  auto const& blockTopo = _topo.modules[FFModuleOsci].block;

  bool on = blockTopo[FFOsciBlockOn].NormalizedToBool(blockState.on[0]);
  if (!on)
  {
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  auto const& gain = accState.gain[0].smoothedCV;
  for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> * _phase);
    output[0][s] = sample * gain[s];
    output[1][s] = sample * gain[s];
    _phase += 440.0f / _sampleRate;
    _phase -= std::floor(_phase);
  }
}