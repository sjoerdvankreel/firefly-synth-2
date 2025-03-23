#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <cmath>

class alignas(sizeof(FBFloatVector)) FFOsciPhase final
{
  float _x = 0.0f;
public:
  FFOsciPhase() = default;
  explicit FFOsciPhase(float x) : _x(x) {}
  FBFloatVector Next(FBFloatVector incr, FBFloatVector fmModulator);
};

inline FBFloatVector
FFOsciPhase::Next(FBFloatVector incr, FBFloatVector fmModulator)
{
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> result;
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> incrScratch;
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> fmModulatorScratch;
  incr.store_aligned(incrScratch.data());
  fmModulator.store_aligned(fmModulatorScratch.data());
  for (int i = 0; i < FBVectorFloatCount; i++)
  {
    float y = _x;
    _x += incrScratch[i] + fmModulatorScratch[i];
    _x -= std::floor(_x);
    assert(0.0f <= _x && _x < 1.0f);
    result[i] = y;
  }
  return FBFloatVector::load_aligned(result.data());
}