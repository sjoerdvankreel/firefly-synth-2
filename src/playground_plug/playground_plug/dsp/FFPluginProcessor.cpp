#include <playground_plug/dsp/FFPluginProcessor.hpp>

#include <cmath>
#include <numbers>

FFPluginProcessor::
FFPluginProcessor(
  FBRuntimeTopo const& topo, int maxHostSampleCount, float sampleRate) :
_phase(),
_sampleRate(sampleRate),
_memory(),
_fixedProcessor(maxHostSampleCount)
{
  topo.InitScalarAddr(_memory);
  topo.InitProcessorAddr(_memory);
}

void 
FFPluginProcessor::ProcessHost(
  FBHostInputBlock const& input, FBRawStereoBlockView& output)
{
  _fixedProcessor.ProcessHost(input, output, *this);
}

void 
FFPluginProcessor::ProcessFixed(
  FBFixedInputBlock const& input, FBFixedStereoBlock& output)
{
  bool on = FBNormalizedToBool(_memory.block.osci.on[0][FFOsciBlockParamOn]);
  if (!on)
  {
    output.Fill(0, FB_FIXED_BLOCK_SIZE, 0.0f);
    return;
  }

  for (int s = 0; s < FB_FIXED_BLOCK_SIZE; s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> * _phase.Current());
    output[FB_CHANNEL_L][s] = sample;
    output[FB_CHANNEL_R][s] = sample;
    _phase.Next(_sampleRate, 440.0f);
  }
}