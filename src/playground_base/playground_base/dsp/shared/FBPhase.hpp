#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <cmath>

class alignas(sizeof(FBFloatVector)) FBPhase final
{
  std::array<float, FBVectorFloatCount> _scratch = {};
  float _x = {};

public:
  FBFloatVector Next(FBFloatVector incr);
};

inline FBFloatVector
FBPhase::Next(FBFloatVector incr)
{
  incr.store_aligned(_scratch.data());
  for (int i = 0; i < FBVectorFloatCount; i++)
  {
    float y = _x;
    _x += _scratch[i];
    _x -= std::floor(_x);
    _scratch[i] = y;
  }
  return FBFloatVector::load_aligned(_scratch.data());
}