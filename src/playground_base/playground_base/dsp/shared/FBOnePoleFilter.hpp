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
  void SetCoeffs(float sampleRate, float durationSecs);
};

inline float
FBOnePoleFilter::Next(float in)
{
  float out = (in * _b) + (_z * _a);
  assert(!std::isnan(out));
  return _z = out;
}

inline void
FBOnePoleFilter::SetCoeffs(float sampleRate, float durationSecs)
{
  _a = std::exp((-2.0f * std::numbers::pi_v<float>) / (durationSecs * sampleRate));
  _b = 1.0f - _a;
}