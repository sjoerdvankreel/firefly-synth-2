#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFNoisePhaseGenerator final
{
  float _x = 0.0f;
public:
  FFNoisePhaseGenerator() = default;
  explicit FFNoisePhaseGenerator(float x) : _x(x) {}
  float Next(float incr);
};

inline float
FFNoisePhaseGenerator::Next(float incr)
{
  float y = _x;
  _x += incr;
  _x = FBPhaseWrap(_x);
  return y;
}