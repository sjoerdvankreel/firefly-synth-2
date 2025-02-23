#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBBarsItem.hpp>

#include <cmath>
#include <numbers>
#include <cassert>

inline constexpr float FBPi = std::numbers::pi_v<float>;
inline constexpr float FBTwoPi = 2.0f * FBPi;

inline FBFloatVector
FBToUnipolar(FBFloatVector v)
{
  return (v + 1.0f) * 0.5f;
}

inline FBFloatVector
FBToBipolar(FBFloatVector v)
{
  return (v * 2.0f) - 1.0f;
}

inline int
FBTimeToSamples(float time, float sampleRate)
{
  return (int)std::round(time * sampleRate);
}

inline int
FBFreqToSamples(float freq, float sampleRate)
{
  assert(freq > 0.0f);
  return (int)std::round(1.0f / freq * sampleRate);
}

inline int
FBBarsToSamples(FBBarsItem const& bars, float sampleRate, float bpm)
{
  return FBTimeToSamples((bars.num * 240.0f) / (bars.denom * bpm), sampleRate);
}

inline float
FBPitchToFreq(float pitch, float sampleRate)
{
  float nyquist(sampleRate * 0.5f);
  float freq(440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f));
  return std::min(nyquist, freq);
}

inline FBFloatVector
FBPitchToFreq(FBFloatVector pitch, float sampleRate)
{ 
  FBFloatVector nyquist(sampleRate * 0.5f);
  FBFloatVector freq(440.0f * xsimd::pow(FBFloatVector(2.0f), (pitch - 69.0f) / 12.0f));
  return xsimd::min(nyquist, freq);
}