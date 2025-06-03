#pragma once

#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <limits>
#include <cstdint>

class FFMarsagliaPRNG final
{
  float _spare = {};
  bool _haveSpare = {};
  FFParkMillerPRNG _uniform = {};

public:
  float NextScalar();

  FFMarsagliaPRNG();
  explicit FFMarsagliaPRNG(float x);
};

inline 
FFMarsagliaPRNG::FFMarsagliaPRNG(float x):
_uniform(x) {}

inline
FFMarsagliaPRNG::FFMarsagliaPRNG() :
_uniform(0.0f) {}

inline float
FFMarsagliaPRNG::NextScalar()
{
  float constexpr mean = 0.0f;
  float constexpr stdev = 1.0f;

  if (_haveSpare)
  {
    _haveSpare = false;
    return _spare * stdev + mean;
  }
  float u, v, s;
  do 
  {
    u = FBToBipolar(_uniform.NextScalar());
    v = FBToBipolar(_uniform.NextScalar());
    s = u * u + v * v;
  } while (s >= 1.0 || s == 0.0);
  s = std::sqrt(-2.0f * std::log(s) / s);
  _spare = v * s;
  _haveSpare = true;
  return mean + stdev * u * s;
}