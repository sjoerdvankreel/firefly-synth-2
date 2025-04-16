#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFOsciPhase final
{
  float _x = 0.0f;
public:
  FFOsciPhase() = default;
  explicit FFOsciPhase(float x) : _x(x) {}
  float Next(float incr, float fmModulator);
};

// vectorize over unison dimension, it's 
// the only option because of feedback fm
class alignas(FBSIMDAlign) FFOsciFMPhases final
{
  xsimd::batch<float, FBXSIMDBatchType> _x = 0.0f;
public:
  FFOsciFMPhases() = default;
  explicit FFOsciFMPhases(xsimd::batch<float, FBXSIMDBatchType> x) : _x(x) {}
  xsimd::batch<float, FBXSIMDBatchType> Next(
    xsimd::batch<float, FBXSIMDBatchType> incrs, 
    xsimd::batch<float, FBXSIMDBatchType> fmModulators);
};

inline float
FFOsciPhase::Next(float incr, float fmModulator)
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

inline xsimd::batch<float, FBXSIMDBatchType>
FFOsciFMPhases::Next(
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