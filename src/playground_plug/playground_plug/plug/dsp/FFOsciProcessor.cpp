#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <cmath>
#include <numbers>

void 
FFOsciProcessor::Process(int moduleSlot, FFPluginBlock& block)
{
  if (FBNormalizedToStep(1, block.paramMemory.osciPlug[moduleSlot][FFOsciPlugParamOn]) == 0)
  {
    block.osciOut[moduleSlot].SetToZero();
    return;
  }

  for (int s = 0; s < FF_BLOCK_SIZE; s++)
  {
    _phase += 440.0f / block.sampleRate;
    _phase -= std::floor(_phase);
    float sample = std::sin(_phase * 2.0f * std::numbers::pi_v<float>);
    for(int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
      block.osciOut[moduleSlot][channel][s] = sample;
  }
}