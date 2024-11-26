#include <playground_plug/dsp/FFPluginProcessor.hpp>

#include <cmath>
#include <numbers>

FFPluginProcessor::
FFPluginProcessor(
  FBRuntimeTopo const& topo, int maxHostSampleCount, float sampleRate) :
FBFixedAutomationProcessor(&_memory, &_memory, maxHostSampleCount),
_phase(),
_sampleRate(sampleRate),
_memory()
{
  topo.InitScalarAddr(_memory);
  topo.InitProcessorAddr(_memory);
}

void 
FFPluginProcessor::ProcessAutomation(
  FBFixedInputBlock const& input, FBFixedAudioBlock& output)
{
  bool on = FBNormalizedToBool(_memory.block.osci.on[0][FFOsciBlockParamOn]);
  if (!on)
  {
    output.Fill(0.0f);
    return;
  }

  // TODO make it good -- both slots
  auto const& gain = _memory.dense.osci.gain[0][0];
  for (int s = 0; s < FB_FIXED_BLOCK_SIZE; s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> * _phase.Current());
    output[FB_CHANNEL_L][s] = sample;// *gain[s]; // TODO nogood with reaper 160 samples and blocksize 2048
    output[FB_CHANNEL_R][s] = sample;// *gain[s];
    _phase.Next(_sampleRate, 440.0f);
  }
}