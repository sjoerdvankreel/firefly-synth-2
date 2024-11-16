#include <playground_plug/plug/shared/FFPluginTopology.hpp>
#include <playground_plug/plug/dsp/FFOscillatorProcessor.hpp>

#include <cmath>
#include <numbers>

void 
FFOscillatorProcessor::Process(int moduleSlot, float sampleRate, FFPluginProcessorBlock& processorBlock)
{
  for (int s = 0; s < FF_BLOCK_SIZE; s++)
  {
    _phase += 440.0f / sampleRate;
    _phase -= std::floor(_phase);
    float sample = std::sin(_phase * 2.0f * std::numbers::pi_v<float>);
    for(int channel = 0; channel < 2; channel++)
      processorBlock.oscillatorAudioOutput[moduleSlot][channel][s] = sample;
  }
}