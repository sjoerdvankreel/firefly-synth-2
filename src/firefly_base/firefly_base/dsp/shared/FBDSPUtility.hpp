#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBBarsItem.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>
#include <numbers>
#include <algorithm>

// Needed for the mseg editor and the actual envelope generator both.
inline constexpr float const FBEnvMinSlope = 0.001f;
inline constexpr float const FBEnvSlopeRange = 1.0f - 2.0f * FBEnvMinSlope;

inline float const FBInvLogHalf = 1.0f / std::log(0.5f);
inline constexpr float FBPi = std::numbers::pi_v<float>;
inline constexpr float FBTwoPi = 2.0f * FBPi;
inline constexpr float FBMaxPitch = 140.0f; // Safety net at 26.5 kHz.
inline constexpr float FBMinPitch = -48.0f; // Very small Hz but not denormal. Also some algorithms divide by frequency => div/0.

template <class T>
inline T
FBToUnipolar(T v)
{
  return (v * 0.5f) + 0.5f;
}

template <class T>
inline T
FBToBipolar(T v)
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

inline float
FBTimeToFloatSamples(float time, float sampleRate)
{
  return time * sampleRate;
}

inline FBBatch<float>
FBTimeToFloatSamples(FBBatch<float> time, FBBatch<float> sampleRate)
{
  return time * sampleRate;
}

inline int
FBFreqToSamples(float freq, float sampleRate)
{
  FB_ASSERT(freq > 0.0f);
  return static_cast<int>(std::round(1.0f / freq * sampleRate));
}

inline float
FBBarsToFreq(FBBarsItem const& bars, float bpm)
{
  // A tempo-synced feature (e.g. the echo module's feedback delay)
  // dividing by an invalid bpm here used to produce inf, which then got
  // cast to int for a delay-buffer tap index in FFDelayLine::Delay() --
  // undefined behavior, and in FFDelayLine's read functions specifically,
  // an out-of-bounds array read (they don't wrap negative indices).
  // Fall back instead, and say so once. The warning itself lives in
  // FBUtility.cpp (FBWarnInvalidBpmOnce), not inline here -- this header
  // is included by ~26 translation units, and a function-local static +
  // logging call inside an inline function duplicates across all of them
  // for the linker to fold, which is unnecessary link-time cost for a
  // one-shot diagnostic.
  if (bpm <= 0.0f)
    FBWarnInvalidBpmOnce();
  float safeBpm = bpm > 0.0f ? bpm : 120.0f;
  return (bars.denom * safeBpm) / (bars.num * 240.0f);
}

inline float
FBBarsToTime(FBBarsItem const& bars, float bpm)
{
  float freq = FBBarsToFreq(bars, bpm);
  return freq > 0.0f ? 1.0f / freq : 1.0f; // never return inf
}

inline int
FBBarsToSamples(FBBarsItem const& bars, float sampleRate, float bpm)
{
  return FBTimeToSamples(FBBarsToTime(bars, bpm), sampleRate);
}

inline float
FBBarsToFloatSamples(FBBarsItem const& bars, float sampleRate, float bpm)
{
  // Was: FBTimeToFloatSamples((bars.num * 240.0f) / (bars.denom * bpm), sampleRate)
  // -- a second, independent bpm division that bypassed FBBarsToFreq's
  // guard entirely. Route through FBBarsToTime instead so there is only
  // one place that ever divides by bpm.
  return FBTimeToFloatSamples(FBBarsToTime(bars, bpm), sampleRate);
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

inline float
FBStereoBalance(int channel, float balance)
{
  FB_ASSERT(channel == 0 || channel == 1);
  FB_ASSERT(-1 <= balance && balance <= 1);
  if (channel == 0 && balance <= 0) return 1.0f;
  if (channel == 1 && balance >= 0) return 1.0f;
  if (channel == 0) return 1.0f - balance;
  return 1.0f + balance;
}