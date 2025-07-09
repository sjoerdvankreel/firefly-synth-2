#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>

// scalar phasegen, not vectorized
class FFScalarPhaseGenerator final
{
  float _x = 0.0f;
public:
  float Next(float incr);
  FFScalarPhaseGenerator() = default;
  explicit FFScalarPhaseGenerator(float x) : _x(x) {}
};

// fake vector over time phasegen
class FFTimeVectorPhaseGenerator final
{
  float _x = 0.0f;
public:
  FFTimeVectorPhaseGenerator() = default;
  explicit FFTimeVectorPhaseGenerator(float x) : _x(x) {}
  FBBatch<float> Next(FBBatch<float> incr);
};

// expensive scalar phasegen, fake vectorized
class FFTrackingPhaseGenerator final
{
  float _x = 0.0f;
  bool _cycledOnce = false;
  int _positionSamplesCurrentCycle = 0;
  int _positionSamplesUpToFirstCycle = 0;

public:
  FFTrackingPhaseGenerator() = default;
  explicit FFTrackingPhaseGenerator(float x) : _x(x) {}
  
  float NextScalar(float incr);
  FBBatch<float> NextBatch(FBBatch<float> incr);

  bool CycledOnce() const { return _cycledOnce; }
  int PositionSamplesCurrentCycle() const { return _positionSamplesCurrentCycle; }
  int PositionSamplesUpToFirstCycle() const { return _positionSamplesUpToFirstCycle; }
};

inline float
FFScalarPhaseGenerator::Next(float incr)
{
  float y = _x;
  _x += incr;
  if (_x >= 1.0f)
    _x = 0.0f;
  return y;
}

inline FBBatch<float>
FFTimeVectorPhaseGenerator::Next(FBBatch<float> incr)
{
  FBSArray<float, FBSIMDFloatCount> yArray;
  FBSArray<float, FBSIMDFloatCount> incrArray;
  incrArray.Store(0, incr);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float y = _x;
    _x += incrArray.Get(i);
    _x = FBPhaseWrap(_x);
    yArray.Set(i, y);
  }
  return yArray.Load(0);
}

inline float
FFTrackingPhaseGenerator::NextScalar(float incr)
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
FFTrackingPhaseGenerator::NextBatch(FBBatch<float> incr)
{
  FBSArray<float, FBSIMDTraits<float>::Size> y;
  FBSArray<float, FBSIMDTraits<float>::Size> x(incr);
  for (int i = 0; i < FBSIMDTraits<float>::Size; i++)
    y.Set(i, NextScalar(x.Get(i)));
  return y.Load(0);
}