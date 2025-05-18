#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFNoisePhaseGenerator final
{
  float _x = 0.0f;
public:
  float Next(float incr);
  FFNoisePhaseGenerator() = default;
  explicit FFNoisePhaseGenerator(float x) : _x(x) {}
};

inline float
FFNoisePhaseGenerator::Next(float incr)
{
  float y = _x;
  _x += incr;
  if (_x >= 1.0f)
    _x = 0.0f;
  return y;
}