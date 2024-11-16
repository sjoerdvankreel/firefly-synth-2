#include <playground_plug/plug/dsp/FFOscillatorProcessor.hpp>
#include <cmath>
#include <numbers>

void 
FFOscillatorProcessor::Process(float sampleRate, FFAudioBlock& block)
{
  for (int s = 0; s < FBBlockSize(block); s++)
  {
    _phase += 440.0f / sampleRate;
    _phase -= std::floor(_phase);
    block[0][s] = block[1][s] = std::sin(_phase * 2.0f * std::numbers::pi_v<float>);
  }
}