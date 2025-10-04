#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

inline float const FFInvLogHalf = 1.0f / std::log(0.5f);

inline float
FFSoftClip10(float val)
{
  if (val > 1.0f || val < -1.0f)
    val = 10.0f * std::tanh(val * 0.1f);
  return val;
}

inline float
FFMultiplyClamp(
  float val, float mul, float min, float max)
{
  return std::clamp(val * mul, min, max);
}

inline float
FFGraphFilterFreqMultiplier(
  bool graph, float sampleRate, float maxFilterFreq)
{
  if (graph && sampleRate < (maxFilterFreq * 2.0f))
    return sampleRate / (maxFilterFreq * 2.0f);
  return 1.0f;
}

inline float 
FFKeyboardTrackingMultiplier(
  float key, float trackingKey, float trackingAmt)
{
  if (trackingAmt == 0.0f)
    return 1.0f;
  return std::pow(2.0f, (key - 60.0f + trackingKey) / 12.0f * trackingAmt);
}

inline FBBatch<float>
FFSkewScaleUnipolar(FBBatch<float> in, FBBatch<float> amt)
{
  return in * (1.0f - amt);
}

inline FBBatch<float>
FFSkewScaleBipolar(FBBatch<float> in, FBBatch<float> amt)
{
  return FBToUnipolar(FBToBipolar(in) * (1.0f - amt));
}

inline FBBatch<float>
FFSkewExpUnipolar(FBBatch<float> in, FBBatch<float> amt)
{
  return xsimd::pow(in, xsimd::log(0.001f + (amt * 0.999f)) * FFInvLogHalf);
}

inline FBBatch<float>
FFSkewExpBipolar(FBBatch<float> in, FBBatch<float> amt)
{
  auto bp = FBToBipolar(in);
  auto exp = xsimd::log(0.001f + (amt * 0.999f)) * FFInvLogHalf;
  return FBToUnipolar(xsimd::sign(bp) * xsimd::pow(xsimd::abs(bp), exp));
}