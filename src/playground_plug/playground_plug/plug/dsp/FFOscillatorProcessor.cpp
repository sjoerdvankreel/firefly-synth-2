#include <playground_plug/plug/dsp/FFOscillatorProcessor.hpp>
#include <cmath>
#include <numbers>

void 
FFOscillatorProcessor::Process(float sampleRate, FFAudioBlock& block)
{
  for (std::size_t sample = 0; sample < FBBlockSize(block); sample++)
  {
    _phase += 440.0f / sampleRate;
    _phase -= std::floor(_phase);
    block[0][sample] = block[1][sample] = std::sin(_phase * 2.0f * std::numbers::pi_v<float>);
  }
}