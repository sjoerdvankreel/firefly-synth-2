#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFOsciMasterPhaseGenerator final
{
  float _x = 0.0f;
public:
  FFOsciMasterPhaseGenerator() = default;
  explicit FFOsciMasterPhaseGenerator(float x) : _x(x) {}
  float Next(float incr, bool& wrapped);
};

class FFOsciSlavePhaseGenerator final
{
  float _x = 0.0f;
public:
  void Set(float p) { _x = p; }
  float Get() const { return _x; }
  float Next(float incr, float fmModulator);
  FFOsciSlavePhaseGenerator() = default;
  explicit FFOsciSlavePhaseGenerator(float x) : _x(x) {}
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
FFOsciMasterPhaseGenerator::Next(float incr, bool& wrapped)
{
  float y = _x;
  _x += incr;
  wrapped = FBPhaseWrap2(_x);
  return y;
}

inline float
FFOsciSlavePhaseGenerator::Next(float incr, float fmModulator)
{
  float y = _x;
  _x += incr;
  FBPhaseWrap(_x);
  y += fmModulator;
  FBPhaseWrap(y);
  return y;
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