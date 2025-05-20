#pragma once

#include <playground_base/base/shared/FBSArray.hpp>

#include <limits>
#include <cstdint>

class FBParkMillerPRNG final
{
  std::uint32_t _x = {};

public:
  float NextScalar();
  FBBatch<float> NextBatch();
  
  FBParkMillerPRNG();
  explicit FBParkMillerPRNG(float x);
};

inline 
FBParkMillerPRNG::FBParkMillerPRNG(float x):
_x(static_cast<std::uint32_t>(1 + x * (std::numeric_limits<std::uint32_t>::max() - 1))) {}

inline
FBParkMillerPRNG::FBParkMillerPRNG() :
FBParkMillerPRNG(0.0f) {}

inline float
FBParkMillerPRNG::NextScalar()
{
  float y = _x / static_cast<float>(std::numeric_limits<std::int32_t>::max());
  _x = static_cast<std::uint64_t>(_x) * 48271 % std::numeric_limits<std::int32_t>::max();
  return y;
}

inline FBBatch<float>
FBParkMillerPRNG::NextBatch()
{
  FBSArray<float, FBSIMDFloatCount> y;
  for (int i = 0; i < FBSIMDFloatCount; i++)
    y.Set(i, NextScalar());
  return y.Load(0);
}
