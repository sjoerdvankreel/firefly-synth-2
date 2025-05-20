#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <limits>
#include <cstdint>

class FBMarsagliaPRNG final
{
  float _spare = {};
  bool _haveSpare = {};
  FBParkMillerPRNG _uniform = {};

public:
  float NextScalar();

  FBMarsagliaPRNG();
  explicit FBMarsagliaPRNG(float x);
};

inline 
FBMarsagliaPRNG::FBMarsagliaPRNG(float x):
_uniform(x) {}

inline
FBMarsagliaPRNG::FBMarsagliaPRNG() :
_uniform(0.0f) {}

inline float
FBMarsagliaPRNG::NextScalar()
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