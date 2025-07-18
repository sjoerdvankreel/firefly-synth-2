#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

// fake vector over time phasegen with fm
class FFOsciWavePhaseGenerator final
{
  float _x = 0.0f;
public:
  FFOsciWavePhaseGenerator() = default;
  explicit FFOsciWavePhaseGenerator(float x) : _x(x) {}
  FBBatch<float> Next(FBBatch<float> incr, FBBatch<float> fmModulator);
};

// real vector over voice phasegen with fm
class alignas(FBSIMDAlign) FFOsciFMPhaseGenerator final
{
  FBBatch<float> _x = 0.0f;
public:
  FFOsciFMPhaseGenerator() = default;
  explicit FFOsciFMPhaseGenerator(FBBatch<float> x) : _x(x) {}
  FBBatch<float> Next(FBBatch<float> incrs, FBBatch<float> fmModulators);
};

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