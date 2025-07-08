#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFOsciStringPhaseGenerator final
{
  float _x = 0.0f;
public:
  float Next(float incr);
  FFOsciStringPhaseGenerator() = default;
  explicit FFOsciStringPhaseGenerator(float x) : _x(x) {}
};

class FFOsciWavePhaseGenerator final
{
  float _x = 0.0f;
public:
  FFOsciWavePhaseGenerator() = default;
  explicit FFOsciWavePhaseGenerator(float x) : _x(x) {}
  FBBatch<float> Next(FBBatch<float> incr, FBBatch<float> fmModulator);
};

class alignas(FBSIMDAlign) FFOsciFMPhaseGenerator final
{
  FBBatch<float> _x = 0.0f;
public:
  FFOsciFMPhaseGenerator() = default;
  explicit FFOsciFMPhaseGenerator(FBBatch<float> x) : _x(x) {}
  FBBatch<float> Next(FBBatch<float> incrs, FBBatch<float> fmModulators);
};

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
  int PositionSamplesCurrentCycle() const { return _positionSamplesCurrentCycle; }
  int PositionSamplesUpToFirstCycle() const { return _positionSamplesUpToFirstCycle; }
};

inline float
FFOsciStringPhaseGenerator::Next(float incr)
{
  float y = _x;
  _x += incr;
  if (_x >= 1.0f)
    _x = 0.0f;
  return y;
}

inline FBBatch<float>
FFOsciFMPhaseGenerator::Next(FBBatch<float> incrs, FBBatch<float> fmModulators)
{
  auto y = _x;
  _x += incrs;
  _x -= xsimd::floor(_x);
  y += fmModulators;
  y -= xsimd::floor(y);
  return y;
}

inline FBBatch<float>
FFOsciWavePhaseGenerator::Next(FBBatch<float> incr, FBBatch<float> fmModulator)
{
  FBSArray<float, FBSIMDFloatCount> yArray;
  FBSArray<float, FBSIMDFloatCount> incrArray;
  FBSArray<float, FBSIMDFloatCount> fmModulatorArray;
  incrArray.Store(0, incr);
  fmModulatorArray.Store(0, fmModulator);
  for (int i = 0; i < FBSIMDFloatCount; i++)
  {
    float y = _x;
    _x += incrArray.Get(i);
    _x = FBPhaseWrap(_x);
    y += fmModulatorArray.Get(i);
    y = FBPhaseWrap(y);
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