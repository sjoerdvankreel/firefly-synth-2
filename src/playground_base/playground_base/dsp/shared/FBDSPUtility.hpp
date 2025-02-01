#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <cmath>
#include <numbers>

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

inline FBFloatVector
FBPitchToFreq(FBFloatVector pitch, float sampleRate)
{ 
  FBFloatVector nyquist(sampleRate * 0.5f);
  FBFloatVector freq(440.0f * xsimd::pow(FBFloatVector(2.0f), (pitch - 69.0f) / 12.0f));
  return xsimd::min(nyquist, freq);
}