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
  float Next(float incr, float fmModulator);
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

inline float
FFOsciPhaseGenerator::Next(float incr, float fmModulator)
{
  float y = _x;
  _x += incr;
  FBPhaseWrap(_x);
  assert(0.0f <= _x && _x < 1.0f);
  y += fmModulator;
  FBPhaseWrap(y);
  assert(0.0f <= y && y < 1.0f);
  return y;
}

inline FBSIMDVector<float> 
FFOsciPhaseGenerator::Next(FBSIMDVector<float> incr, FBSIMDVector<float> fmModulator)
{
  FBSIMDArray<float, FBSIMDTraits<float>::Size> y;
  FBSIMDArray<float, FBSIMDTraits<float>::Size> incrArray;
  FBSIMDArray<float, FBSIMDTraits<float>::Size> fmModulatorArray;
  incrArray.Store(0, incr);
  fmModulatorArray.Store(0, fmModulator);
  for (int i = 0; i < FBSIMDTraits<float>::Size; i++)
    y.Set(i, Next(incrArray.Get(i), fmModulatorArray.Get(i)));
  return y.Load(0);
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