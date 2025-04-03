#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <cmath>

class FFOsciPhase final
{
  float _x = 0.0f;
public:
  FFOsciPhase() = default;
  explicit FFOsciPhase(float x) : _x(x) {}
  float Next(float incr, float fmModulator);
};

inline float
FFOsciPhase::Next(float incr, float fmModulator)
{
  float y = _x;
  _x += incr + fmModulator;
  FBPhaseWrap(_x);
  assert(0.0f <= _x && _x < 1.0f);
  return y;
}