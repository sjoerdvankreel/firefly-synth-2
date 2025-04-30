#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
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

// vectorize over unison dimension, it's 
// the only option because of feedback fm
class alignas(FBSIMDAlign) FFOsciFMPhasesGenerator final
{
  xsimd::batch<float, FBXSIMDBatchType> _x = 0.0f;
public:
  FFOsciFMPhasesGenerator() = default;
  explicit FFOsciFMPhasesGenerator(xsimd::batch<float, FBXSIMDBatchType> x) : _x(x) {}
  xsimd::batch<float, FBXSIMDBatchType> Next(
    xsimd::batch<float, FBXSIMDBatchType> incrs, 
    xsimd::batch<float, FBXSIMDBatchType> fmModulators);
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

inline xsimd::batch<float, FBXSIMDBatchType>
FFOsciFMPhasesGenerator::Next(
  xsimd::batch<float, FBXSIMDBatchType> incrs, 
  xsimd::batch<float, FBXSIMDBatchType> fmModulators)
{
  xsimd::batch<float, FBXSIMDBatchType> y = _x;
  _x += incrs;
  _x -= xsimd::floor(_x);
  y += fmModulators;
  y -= xsimd::floor(y);
  return y;
}