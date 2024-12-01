#pragma once

#include <cmath>
#include <numbers>

class FBOnePoleFilter
{
  float a = 0.0f;
  float b = 0.0f;
  float z = 0.0f;

public:
  float Next(float in)
  { return z = (in * b) + (z * a); }

  FBOnePoleFilter() = default;
  FBOnePoleFilter(float sampleRate, float durationSecs):
  a(std::exp((-2.0f * std::numbers::pi_v<float>) / (durationSecs * sampleRate))),
  b(1.0f - a), z(0.0f) {}
};