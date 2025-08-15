#pragma once

#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <limits>
#include <cstdint>

template <bool Bipolar>
class FFMarsagliaPRNG final
{
  float _spare = {};
  bool _haveSpare = {};
  FFParkMillerPRNG _uniform = {};

  float NextScalarGaussian();

public:
  float NextScalar();
  FBBatch<float> NextBatch();

  FFMarsagliaPRNG();
  explicit FFMarsagliaPRNG(float x);
  explicit FFMarsagliaPRNG(std::uint32_t x);
};

template <bool Bipolar>
inline
FFMarsagliaPRNG<Bipolar>::FFMarsagliaPRNG() :
_uniform(0.0f) {}

template <bool Bipolar>
inline 
FFMarsagliaPRNG<Bipolar>::FFMarsagliaPRNG(float x):
_uniform(x) {}

template <bool Bipolar>
inline 
FFMarsagliaPRNG<Bipolar>::FFMarsagliaPRNG(std::uint32_t x):
_uniform(x) {}

template <bool Bipolar>
inline float
FFMarsagliaPRNG<Bipolar>::NextScalarGaussian()
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

template <bool Bipolar>
inline float
FFMarsagliaPRNG<Bipolar>::NextScalar()
{
  float result = 0.0f;
  do
  {
    result = NextScalarGaussian();
  } while (result < -3.0f || result > 3.0f);
  result /= 3.0f;
  if constexpr (!Bipolar)
    result = FBToUnipolar(result);
  return result;
}

template <bool Bipolar>
inline FBBatch<float>
FFMarsagliaPRNG<Bipolar>::NextBatch()
{
  FBSArray<float, FBSIMDFloatCount> y;
  for (int i = 0; i < FBSIMDFloatCount; i++)
    y.Set(i, NextScalar());
  return y.Load(0);
}