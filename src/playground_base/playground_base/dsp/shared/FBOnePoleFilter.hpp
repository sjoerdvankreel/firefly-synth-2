#pragma once

#include <cmath>
#include <cassert>
#include <numbers>

class FBOnePoleFilter final
{
  float a = 0.0f;
  float b = 0.0f;
  float z = 0.0f;

public:
  float Next(float in);
  FBOnePoleFilter() = default;
  FBOnePoleFilter(float sampleRate, float durationSecs);
};

inline float
FBOnePoleFilter::Next(float in)
{
  float out = (in * b) + (z * a);
  assert(!std::isnan(out));
  return z = out;
}

inline 
FBOnePoleFilter::
FBOnePoleFilter(float sampleRate, float durationSecs):
a(std::exp((-2.0f * std::numbers::pi_v<float>) / (durationSecs * sampleRate))),
b(1.0f - a), z(0.0f) {}