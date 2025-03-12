#pragma once

#include <limits>
#include <cstdint>

class FBParkMillerPRNG final
{
  std::uint32_t _x = {};

public:
  float Next();
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
FBParkMillerPRNG::Next()
{
  float y = _x / static_cast<float>(std::numeric_limits<std::uint32_t>::max());
  _x = static_cast<std::uint64_t>(_x) * 48271 % 0x7fffffff;
  return y;
}
