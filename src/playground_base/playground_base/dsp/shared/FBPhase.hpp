#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <cmath>

class alignas(sizeof(FBFloatVector)) FBPhase final
{
  float _x = 0.0f;
  bool _cycledOnce = false;
  int _positionSamplesCurrentCycle = 0;
  int _positionSamplesUpToFirstCycle = 0;
public:
  FBPhase() = default;
  explicit FBPhase(float x) : _x(x) {}
  FBFloatVector Next(FBFloatVector incr);
  int PositionSamplesCurrentCycle() const { return _positionSamplesCurrentCycle; }
  int PositionSamplesUpToFirstCycle() const { return _positionSamplesUpToFirstCycle; }
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
    float f = std::floor(_x);
    if (f != 0.0f)
    {
      _cycledOnce = true;
      _positionSamplesCurrentCycle = 0;
    }
    else
    {
      _positionSamplesCurrentCycle++;
      if (!_cycledOnce)
        _positionSamplesUpToFirstCycle++;
    }
    _x -= f;
    scratch[i] = y;
  }
  return FBFloatVector::load_aligned(scratch.data());
}