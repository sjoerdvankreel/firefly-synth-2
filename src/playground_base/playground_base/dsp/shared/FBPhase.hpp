#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <cmath>

class alignas(sizeof(FBFloatVector)) FBPhase final
{
  float _x = 0.0f;

public:
  FBFloatVector Next(FBFloatVector incr);
};

inline FBFloatVector
FBPhase::Next(FBFloatVector incr)
{
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> scratch;
  incr.store_aligned(scratch.data());
  for (int i = 0; i < FBVectorFloatCount; i++)
  {
    float y = _x;
    _x += scratch[i];
    _x -= std::floor(_x);
    scratch[i] = y;
  }
  return FBFloatVector::load_aligned(scratch.data());
}