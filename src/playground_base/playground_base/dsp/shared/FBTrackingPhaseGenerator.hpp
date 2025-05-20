#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>

class FBTrackingPhaseGenerator final
{
  float _x = 0.0f;
  bool _cycledOnce = false;
  int _positionSamplesCurrentCycle = 0;
  int _positionSamplesUpToFirstCycle = 0;

public:
  FBTrackingPhaseGenerator() = default;
  explicit FBTrackingPhaseGenerator(float x) : _x(x) {}
  
  float NextScalar(float incr);
  FBBatch<float> NextBatch(FBBatch<float> incr);
  int PositionSamplesCurrentCycle() const { return _positionSamplesCurrentCycle; }
  int PositionSamplesUpToFirstCycle() const { return _positionSamplesUpToFirstCycle; }
};

inline float
FBTrackingPhaseGenerator::NextScalar(float incr)
{
  float y = _x;
  _x += incr;
  bool w = FBPhaseWrap2(_x);
  if (w)
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
  return y;
}

inline FBBatch<float>
FBTrackingPhaseGenerator::NextBatch(FBBatch<float> incr)
{
  FBArray<float, FBSIMDTraits<float>::Size> y;
  FBArray<float, FBSIMDTraits<float>::Size> x(incr);
  for (int i = 0; i < FBSIMDTraits<float>::Size; i++)
    y.Set(i, NextScalar(x.Get(i)));
  return y.Load(0);
}