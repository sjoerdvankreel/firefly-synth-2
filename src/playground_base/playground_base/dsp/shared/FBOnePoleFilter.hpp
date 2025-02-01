#pragma once

#include <cmath>
#include <cassert>
#include <numbers>

class FBOnePoleFilter final
{
  float _a = 0.0f;
  float _b = 0.0f; 
  float _z = 0.0f;

public:
  float Next(float in);
  void SetCoeffs(int sampleCount);

  float State() const { return _z; }
  void State(float state) { _z = state; }
};

inline float
FBOnePoleFilter::Next(float in)
{
  float out = (in * _b) + (_z * _a);
  assert(!std::isnan(out));
  return _z = out;
}

inline void
FBOnePoleFilter::SetCoeffs(int sampleCount)
{
  _a = std::exp((-2.0f * std::numbers::pi_v<float>) / (float)sampleCount);
  _b = 1.0f - _a;
}