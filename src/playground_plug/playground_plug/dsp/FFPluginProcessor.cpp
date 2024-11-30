#include <playground_plug/dsp/FFPluginProcessor.hpp>

#include <cmath>
#include <numbers>

FFPluginProcessor::
FFPluginProcessor(
  FBRuntimeTopo const& topo, int maxHostSampleCount, float sampleRate) :
FBAutomationRamper(&_memory, &_memory, maxHostSampleCount),
_phase(),
_memory(),
_sampleRate(sampleRate)
{
  topo.InitProcAddrs(_memory);
  topo.InitScalarAddrs(_memory);
}

void 
FFPluginProcessor::ProcessAutomation(
  FBPlugInputBlock const& input, FBPlugAudioBlock& output)
{
  bool on = FBNormalizedToBool(_memory.block.osci.on[0][FFOsciBlockOn]);
  if (!on)
  {
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  // TODO make it good -- both slots
  auto const& gain = _memory.cv.osci.gain[0][0];
  for (int s = 0; s < FB_PLUG_BLOCK_SIZE; s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> * _phase.Current());
    output[0][s] = sample *gain[s];
    output[1][s] = sample *gain[s];
    _phase.Next(_sampleRate, 440.0f);
  }
}