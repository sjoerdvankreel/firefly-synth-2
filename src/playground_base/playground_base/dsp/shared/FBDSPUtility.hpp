#pragma once

#include <cmath>
#include <numbers>

inline float 
FBPitchToFreq(float pitch)
{ return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f); }

inline float
FBPhaseToSine(float phase)
{ return std::sin(phase * 2.0f * std::numbers::pi_v<float>); }