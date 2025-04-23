#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
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
  
  float Next(float incr);
  void Next(FBFixedFloatArray const& incr); // todo drop ?
  FBXSIMDFloatBatch Next(FBXSIMDFloatBatch incr);
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

inline FBXSIMDFloatBatch 
FBTrackingPhaseGenerator::Next(FBXSIMDFloatBatch incr)
{
  alignas(FBSIMDAlign) std::array<float, FBSIMDFloatCount> x;
  alignas(FBSIMDAlign) std::array<float, FBSIMDFloatCount> y;
  incr.store_aligned(x.data());
  for (int i = 0; i < FBSIMDFloatCount; i++)
    y[i] = Next(x[i]);
  return FBLoadSIMDAligned(y.data());
}