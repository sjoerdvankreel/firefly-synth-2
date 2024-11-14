#include <playground_plug/plug.hpp>
#include <cmath>
#include <numbers>

void 
process(float const* FF_RESTRICT const* FF_RESTRICT in, float* FF_RESTRICT const* FF_RESTRICT out, int n)
{
  static float p = 0.0f;
  for (int i = 0; i < n; i++)
  {
    p += 440.0f / 48000.0f;
    out[0][i] = out[1][i] = std::sin(p * 2.0f * std::numbers::pi_v<float>);
  }
}