#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>

#include <cmath>
#include <numbers>

void 
FFPluginProcessor::Process(FF_RAW_INPUT_BUFFER in, FF_RAW_OUTPUT_BUFFER out, std::size_t n)
{
  static float p = 0.0f;
  for (std::size_t i = 0; i < n; i++)
  {
    p += 440.0f / 48000.0f;
    out[0][i] = out[1][i] = std::sin(p * 2.0f * std::numbers::pi_v<float>);
  }
}