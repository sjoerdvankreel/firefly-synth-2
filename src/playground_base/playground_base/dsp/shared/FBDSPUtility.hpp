#pragma once

#include <cmath>

inline float 
FBPitchToFreq(float pitch)
{ return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f); }