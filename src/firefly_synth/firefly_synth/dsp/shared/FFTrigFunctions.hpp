#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

enum FFTrigFunction {
  FFTrigSin, FFTrigCos, 
  FFTrigSin2, FFTrigCos2, FFTrigSinCos, FFTrigCosSin, 
  FFTrigSin3, FFTrigCos3, FFTrigSn2Cs, FFTrigCs2Sn,
  FFTrigSnCs2, FFTrigCsSn2, FFTrigSnCsSn, FFTrigCsSnCs,
  FFTrigCount 
};

inline FBBatch<float>
FFCalcTrigSin(FBBatch<float> in)
{
  return xsimd::sin(in);
}

inline FBBatch<float>
FFCalcTrigCos(FBBatch<float> in)
{
  return xsimd::cos(in);
}

inline FBBatch<float>
FFCalcTrigSin2(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in);
  return xsimd::sin(in + sin1);
}

inline FBBatch<float>
FFCalcTrigCos2(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in);
  return xsimd::cos(in + cos1);
}

inline FBBatch<float>
FFCalcTrigSinCos(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in);
  return xsimd::sin(in + cos1);
}

inline FBBatch<float>
FFCalcTrigCosSin(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in);
  return xsimd::cos(in + sin1);
}

inline FBBatch<float>
FFCalcTrigSin3(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in);
  auto sin2 = xsimd::sin(in + sin1);
  return xsimd::sin(in + sin2);
}

inline FBBatch<float>
FFCalcTrigCos3(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in);
  auto cos2 = xsimd::cos(in + cos1);
  return xsimd::cos(in + cos2);
}

inline FBBatch<float>
FFCalcTrigSn2Cs(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in);
  auto sin2 = xsimd::sin(in + cos1);
  return xsimd::sin(in + sin2);
}

inline FBBatch<float>
FFCalcTrigCs2Sn(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in);
  auto cos2 = xsimd::cos(in + sin1);
  return xsimd::cos(in + cos2);
}

inline FBBatch<float>
FFCalcTrigSnCs2(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in);
  auto cos2 = xsimd::cos(in + cos1);
  return xsimd::sin(in + cos2);
}

inline FBBatch<float>
FFCalcTrigCsSn2(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in);
  auto sin2 = xsimd::sin(in + sin1);
  return xsimd::cos(in + sin2);
}

inline FBBatch<float>
FFCalcTrigSnCsSn(FBBatch<float> in)
{
  auto sin1 = xsimd::sin(in);
  auto cos2 = xsimd::cos(in + sin1);
  return xsimd::sin(in + cos2);
}

inline FBBatch<float>
FFCalcTrigCsSnCs(FBBatch<float> in)
{
  auto cos1 = xsimd::cos(in);
  auto sin2 = xsimd::sin(in + cos1);
  return xsimd::cos(in + sin2);
}

inline FBBatch<float>
FFCalcTrig(int type, FBBatch<float> in)
{
  FB_ASSERT(FFTrigSin <= type && type < FFTrigCount);
  switch (type)
  {
  case FFTrigSin: return FFCalcTrigSin(in);
  case FFTrigCos: return FFCalcTrigCos(in);
  case FFTrigSin2: return FFCalcTrigSin2(in);
  case FFTrigCos2: return FFCalcTrigCos2(in);
  case FFTrigSinCos: return FFCalcTrigSinCos(in);
  case FFTrigCosSin: return FFCalcTrigCosSin(in);
  case FFTrigSin3: return FFCalcTrigSin3(in);
  case FFTrigCos3: return FFCalcTrigCos3(in);
  case FFTrigSn2Cs: return FFCalcTrigSn2Cs(in);
  case FFTrigCs2Sn: return FFCalcTrigCs2Sn(in);
  case FFTrigSnCs2: return FFCalcTrigSnCs2(in);
  case FFTrigCsSn2: return FFCalcTrigCsSn2(in);
  case FFTrigSnCsSn: return FFCalcTrigSnCsSn(in);
  case FFTrigCsSnCs: return FFCalcTrigCsSnCs(in);
  default: FB_ASSERT(false); return {};    
  }
}