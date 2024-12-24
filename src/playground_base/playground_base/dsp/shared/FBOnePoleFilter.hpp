#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

class alignas(sizeof(FBFloatVector)) FBOnePoleFilter final
{
  std::array<float, FBVectorFloatCount> _scratch = {};
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
  in.store_aligned(_scratch.data());
  for (int i = 0; i < FBVectorFloatCount; i++)
  {
    _z = (_scratch[i] * _b) + (_z * _a);
    _scratch[i] = _z;
  }
  return FBFloatVector::load_aligned(_scratch.data());
}

inline void
FBOnePoleFilter::SetCoeffs(float sampleRate, float durationSecs)
{
  _a = std::exp((-2.0f * std::numbers::pi_v<float>) / (durationSecs * sampleRate));
  _b = 1.0f - _a;
}