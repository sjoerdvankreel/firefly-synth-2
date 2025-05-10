#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFOsciPhaseGenerator final
{
  float _x = 0.0f;
public:
  FFOsciPhaseGenerator() = default;
  explicit FFOsciPhaseGenerator(float x) : _x(x) {}
  FBSIMDVector<float> Next(FBSIMDVector<float> incr, FBSIMDVector<float> fmModulator);
};

class alignas(FBSIMDAlign) FFOsciFMPhaseGenerator final
{
  FBSIMDVector<float> _x = 0.0f;
public:
  FFOsciFMPhaseGenerator() = default;
  explicit FFOsciFMPhaseGenerator(FBSIMDVector<float> x) : _x(x) {}
  FBSIMDVector<float> Next(FBSIMDVector<float> incrs, FBSIMDVector<float> fmModulators);
};

inline FBSIMDVector<float> 
FFOsciPhaseGenerator::Next(FBSIMDVector<float> incr, FBSIMDVector<float> fmModulator)
{
  FBSIMDArray<float, FBSIMDFloatCount> yArray;
  FBSIMDArray<float, FBSIMDFloatCount> incrArray;
  FBSIMDArray<float, FBSIMDFloatCount> fmModulatorArray;
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

inline FBSIMDVector<float>
FFOsciFMPhaseGenerator::Next(FBSIMDVector<float> incrs, FBSIMDVector<float> fmModulators)
{
  auto y = _x;
  _x += incrs;
  _x -= xsimd::floor(_x);
  y += fmModulators;
  y -= xsimd::floor(y);
  return y;
}