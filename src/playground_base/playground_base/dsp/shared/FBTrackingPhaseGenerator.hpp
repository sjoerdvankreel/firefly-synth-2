#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>

class FBTrackingPhaseGenerator final
{
  float _x = 0.0f;
  bool _cycledOnce = false;
  int _positionSamplesCurrentCycle = 0; // todo drop
  int _positionSamplesUpToFirstCycle = 0;

public:
  FBTrackingPhaseGenerator() = default;
  explicit FBTrackingPhaseGenerator(float x) : _x(x) {}
  
  float Next(float incr);
  void Next(FBFixedFloatArray const& incr); // todo drop
  FBSIMDVector<float> Next(FBSIMDVector<float> incr);
  int PositionSamplesCurrentCycle() const { return _positionSamplesCurrentCycle; }
  int PositionSamplesUpToFirstCycle() const { return _positionSamplesUpToFirstCycle; }
};

inline float
FBTrackingPhaseGenerator::Next(float incr)
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

inline void
FBTrackingPhaseGenerator::Next(FBFixedFloatArray const& incr)
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    Next(incr[s]);
}

inline FBSIMDVector<float> 
FBTrackingPhaseGenerator::Next(FBSIMDVector<float> incr)
{
  FBSIMDArray<float, FBSIMDTraits<float>::Size> y;
  FBSIMDArray<float, FBSIMDTraits<float>::Size> x(incr);
  for (int i = 0; i < FBSIMDTraits<float>::Size; i++)
    y.Set(i, Next(x.Get(i)));
  return y.Load(0);
}