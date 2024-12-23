#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <cmath>
#include <numbers>

inline constexpr float FBPi = std::numbers::pi_v<float>;
inline constexpr float FBTwoPi = 2.0f * FBPi;

inline float 
FBPitchToFreq(float pitch)
{ return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f); } // TODO vector

inline FBFloatVector
FBPitchToFreq(FBFloatVector pitch)
{ return 440.0f * FBFloatVectorPow(2.0f, (pitch - 69.0f) / 12.0f); }