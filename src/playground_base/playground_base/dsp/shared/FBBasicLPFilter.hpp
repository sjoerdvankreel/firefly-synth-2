#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

// https://www.musicdsp.org/en/latest/Filters/257-1-pole-lpf-for-smooth-parameter-changes.html
class FBBasicLPFilter final
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
FBBasicLPFilter::Next(float in)
{
  float out = (in * _b) + (_z * _a);
  assert(!std::isnan(out));
  return _z = out;
}

inline void
FBBasicLPFilter::SetCoeffs(int sampleCount)
{
  _a = std::exp((-2.0f * FBPi) / static_cast<float>(sampleCount));
  _b = 1.0f - _a;
}