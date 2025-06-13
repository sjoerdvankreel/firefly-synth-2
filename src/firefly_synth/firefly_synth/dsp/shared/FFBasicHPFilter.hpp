#pragma once

#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>
#include <cassert>
#include <numbers>

// https://www.musicdsp.org/en/latest/Filters/117-one-pole-one-zero-lp-hp.html
class FFBasicHPFilter final
{
  float _a0 = 0.0f;
  float _a1 = 0.0f; 
  float _b1 = 0.0f;
  float _x1 = 0.0f;
  float _y1 = 0.0f;

public:
  float Next(float in);
  void SetCoeffs(float cutoffFreq, float sampleRate);
};

inline float
FFBasicHPFilter::Next(float in)
{
  float out = _a0 * in + _a1 * _x1 + _b1 * _y1;
  FB_ASSERT(!std::isnan(out));
  _x1 = in;
  _y1 = out;
  return out;
}

inline void
FFBasicHPFilter::SetCoeffs(float cutoffFreq, float sampleRate)
{
  float w = 2.0f * sampleRate;
  float cutoffFreqW = cutoffFreq * FBTwoPi;
  float n = 1.0f / (cutoffFreqW + w);
  _a0 = w * n;
  _a1 = -_a0;
  _b1 = (w - cutoffFreqW) * n;
}