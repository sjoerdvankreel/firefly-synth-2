#pragma once

#include <playground_base/base/topo/param/FBBarsItem.hpp>

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
  return (v + 1.0f) * 0.5f;
}

inline float
FBToBipolar(float v)
{
  return (v * 2.0f) - 1.0f;
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
FBPitchToFreqAccurate(float pitch, float sampleRate)
{
  float freq = 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f);
  return std::min(sampleRate * 0.5f, freq);
}