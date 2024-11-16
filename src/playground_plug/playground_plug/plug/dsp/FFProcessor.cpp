#include <playground_plug/plug/dsp/FFProcessor.hpp>
#include <cmath>
#include <numbers>

FFProcessor::
FFProcessor(float sampleRate) :
_sampleRate(sampleRate)
{
}

void 
FFProcessor::Process(FB_RAW_INPUT_BUFFER in, FB_RAW_OUTPUT_BUFFER out, std::size_t n)
{
  for (std::size_t i = 0; i < n; i++)
  {
    _phase += 440.0f / _sampleRate;
    _phase -= std::floor(_phase);
    out[0][i] = out[1][i] = std::sin(_phase * 2.0f * std::numbers::pi_v<float>);
  }
}