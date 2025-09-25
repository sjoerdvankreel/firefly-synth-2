#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>
#include <numbers>

// https://www.musicdsp.org/en/latest/Filters/257-1-pole-lpf-for-smooth-parameter-changes.html
class FBBasicLPFilter final
{
  float _a = 0.0f;
  float _b = 0.0f; 
  float _z = 0.0f;

public:
  void SetCoeffs(int sampleCount);
  float State() const { return _z; }
  void State(float state) { _z = state; }

  float NextScalar(float in);
  FBBatch<float> NextBatch(FBBatch<float> in);
};

inline float
FBBasicLPFilter::NextScalar(float in)
{
  float out = (in * _b) + (_z * _a);
  FB_ASSERT(!std::isnan(out));
  return _z = out;
}

inline FBBatch<float>
FBBasicLPFilter::NextBatch(FBBatch<float> in)
{
  FBSArray<float, FBSIMDFloatCount> arr;
  arr.Store(0, in);
  for (int i = 0; i < FBSIMDFloatCount; i++)
    arr.Set(i, NextScalar(arr.Get(i)));
  return arr.Load(0);
}

inline void
FBBasicLPFilter::SetCoeffs(int sampleCount)
{
  _a = std::exp((-2.0f * FBPi) / static_cast<float>(sampleCount));
  _b = 1.0f - _a;
}