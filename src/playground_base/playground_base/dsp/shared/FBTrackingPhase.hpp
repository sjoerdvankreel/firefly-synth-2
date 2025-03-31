#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <cmath>

class FBTrackingPhase final
{
  float _x = 0.0f;
  bool _cycledOnce = false;
  int _positionSamplesCurrentCycle = 0;
  int _positionSamplesUpToFirstCycle = 0;

public:
  FBTrackingPhase() = default;
  explicit FBTrackingPhase(float x) : _x(x) {}
  
  float Next(float incr);
  void Next(FBFixedFloatArray const& incr);
  int PositionSamplesCurrentCycle() const { return _positionSamplesCurrentCycle; }
  int PositionSamplesUpToFirstCycle() const { return _positionSamplesUpToFirstCycle; }
};

inline float
FBTrackingPhase::Next(float incr)
{
  float y = _x;
  _x += incr;
  float f = std::floor(_x);
  _x -= f;
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
  return y;
}

inline void
FBTrackingPhase::Next(FBFixedFloatArray const& incr)
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    Next(incr[s]);
}