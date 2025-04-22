#pragma once

#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/base/topo/param/FBBarsItem.hpp>
#include <xsimd/xsimd.hpp>

#include <cmath>
#include <array>
#include <numbers>
#include <cassert>
#include <algorithm>

inline constexpr float FBPi = std::numbers::pi_v<float>;
inline constexpr float FBTwoPi = 2.0f * FBPi;

inline float
FBToUnipolar(float v)
{
  return (v * 0.5f) + 0.5f;
}

inline float
FBToBipolar(float v)
{
  return (v - 0.5f) * 2.0f;
}

// https://stackoverflow.com/questions/824118/why-is-floor-so-slow
inline int
FBFastFloor(float x)
{
  int i = (int)x;
  return i - (i > x);
}

inline void
FBPhaseWrap(float& p)
{
  if (p >= 1.0f || p < 0.0f)
    p -= FBFastFloor(p);
}

inline bool
FBPhaseWrap2(float& p)
{
  if (p >= 1.0f || p < 0.0f)
  {
    p -= FBFastFloor(p);
    return true;
  }
  return false;
}

inline int
FBTimeToSamples(float time, float sampleRate)
{
  return static_cast<int>(std::round(time * sampleRate));
}

inline int
FBFreqToSamples(float freq, float sampleRate)
{
  assert(freq > 0.0f);
  return static_cast<int>(std::round(1.0f / freq * sampleRate));
}

inline int
FBBarsToSamples(FBBarsItem const& bars, float sampleRate, float bpm)
{
  return FBTimeToSamples((bars.num * 240.0f) / (bars.denom * bpm), sampleRate);
}

inline float
FBPitchToFreq(float pitch)
{
  return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f);
}

inline xsimd::batch<float, FBXSIMDBatchType>
FBPitchToFreq(xsimd::batch<float, FBXSIMDBatchType> pitch)
{
  auto two = xsimd::batch<float, FBXSIMDBatchType>(2.0f);
  auto exp = (pitch - 69.0f) / 12.0f;
  return 440.0f * xsimd::pow(two, exp);
}