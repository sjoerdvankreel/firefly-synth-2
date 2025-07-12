#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>

#include <limits>
#include <cstdint>

class FFParkMillerPRNG final
{
  std::uint32_t _state = {};

public:
  float NextScalar();
  FBBatch<float> NextBatch();
  std::uint32_t State() const { return _state; }

  FFParkMillerPRNG();
  explicit FFParkMillerPRNG(float x);
  explicit FFParkMillerPRNG(std::uint32_t x);
};

inline
FFParkMillerPRNG::FFParkMillerPRNG(std::uint32_t seed):
_state(seed) {}

inline 
FFParkMillerPRNG::FFParkMillerPRNG(float seed):
_state(static_cast<std::uint32_t>(1 + seed * (std::numeric_limits<std::uint32_t>::max() - 1))) {}

inline
FFParkMillerPRNG::FFParkMillerPRNG() :
FFParkMillerPRNG(0.0f) {}

inline float
FFParkMillerPRNG::NextScalar()
{
  float y = _state / static_cast<float>(std::numeric_limits<std::int32_t>::max());
  _state = static_cast<std::uint64_t>(_state) * 48271 % std::numeric_limits<std::int32_t>::max();
  return y;
}

inline FBBatch<float>
FFParkMillerPRNG::NextBatch()
{
  FBSArray<float, FBSIMDFloatCount> y;
  for (int i = 0; i < FBSIMDFloatCount; i++)
    y.Set(i, NextScalar());
  return y.Load(0);
}
