#pragma once

#include <cmath>
#include <numbers>

inline float
FBUnipolarToBiPolar(float val)
{ return val * 2.0f - 1.0f; }
inline float
FBBiPolarToUnipolar(float val)
{ return (val + 1.0f) * 0.5f; }

inline float 
FBPitchToFreq(float pitch)
{ return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f); }
inline float
FBPhaseToSine(float phase)
{ return std::sin(phase * 2.0f * std::numbers::pi_v<float>); }