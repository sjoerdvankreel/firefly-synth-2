#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

class FBOnePoleFilter final
{
  float _a = 0.0f;
  float _b = 0.0f; 
  float _z = 0.0f;

public:
  FBFloatVector Next(FBFloatVector in);
  void SetCoeffs(float sampleRate, float durationSecs);
};

inline FBFloatVector
FBOnePoleFilter::Next(FBFloatVector in)
{
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> scratch;
  in.store_aligned(scratch.data());
  for (int i = 0; i < FBVectorFloatCount; i++)
  {
    _z = (scratch[i] * _b) + (_z * _a);
    scratch[i] = _z;
  }
  return FBFloatVector::load_aligned(scratch.data());
}

inline void
FBOnePoleFilter::SetCoeffs(float sampleRate, float durationSecs)
{
  _a = std::exp((-2.0f * std::numbers::pi_v<float>) / (durationSecs * sampleRate));
  _b = 1.0f - _a;
}