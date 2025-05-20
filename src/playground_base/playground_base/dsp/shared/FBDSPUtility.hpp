#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/topo/static/FBBarsItem.hpp>
#include <xsimd/xsimd.hpp>

#include <cmath>
#include <array>
#include <numbers>
#include <cassert>
#include <algorithm>

inline constexpr float FBPi = std::numbers::pi_v<float>;
inline constexpr float FBTwoPi = 2.0f * FBPi;
inline constexpr float FBMaxPitch = 140.0f; // Safety net at 26.5 kHz.
inline constexpr float FBMinPitch = -48.0f; // Very small Hz but not denormal. Also some algorithms divide by frequency => div/0.

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

inline FBBatch<float>
FBToUnipolar(FBBatch<float> v)
{
  return (v * 0.5f) + 0.5f;
} 

inline FBBatch<float>
FBToBipolar(FBBatch<float> v)
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

inline float
FBPhaseWrap(float p)
{
  if (p >= 1.0f || p < 0.0f)
    p -= FBFastFloor(p);
  return p;
}

inline FBBatch<float>
FBPhaseWrap(FBBatch<float> p)
{
  return p - xsimd::floor(p);
}

inline bool
FBPhaseWrap2(float& p)
{
  if (p >= 0.0f && p < 1.0f)
    return false;
  p -= FBFastFloor(p);
  return true;
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
  pitch = std::clamp(pitch, FBMinPitch, FBMaxPitch);
  return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f);
}

inline FBBatch<float>
FBPitchToFreq(FBBatch<float> pitch)
{
  pitch = xsimd::clip(pitch, FBBatch<float>(FBMinPitch), FBBatch<float>(FBMaxPitch));
  return 440.0f * xsimd::pow(FBBatch<float>(2.0f), (pitch - 69.0f) / 12.0f);
}