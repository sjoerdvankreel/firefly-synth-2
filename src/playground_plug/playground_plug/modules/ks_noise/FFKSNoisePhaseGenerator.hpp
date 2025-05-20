#pragma once

#include <playground_base/base/shared/FBArray.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFKSNoisePhaseGenerator final
{
  float _x = 0.0f;
public:
  float Next(float incr);
  FFKSNoisePhaseGenerator() = default;
  explicit FFKSNoisePhaseGenerator(float x) : _x(x) {}
};

inline float
FFKSNoisePhaseGenerator::Next(float incr)
{
  float y = _x;
  _x += incr;
  if (_x >= 1.0f)
    _x = 0.0f;
  return y;
}