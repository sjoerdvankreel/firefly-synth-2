#pragma once

#include <firefly_synth/dsp/shared/FFParkMillerPRNG.hpp>
#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <array>
#include <cassert>
#include <cstdint>

// https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/procedural-patterns-noise-part-1/creating-simple-1D-noise.html

inline int constexpr FFNoiseGeneratorMaxSteps = 100;

template <bool Smooth>
class FFNoiseGenerator
{
  int _steps = 2;
  FFParkMillerPRNG _prng = {};
  std::array<float, FFNoiseGeneratorMaxSteps> _r = {};

  void InitSteps(int steps);
  float Lerp(float lo, float hi, float t) const;
  float CosineRemap(float a, float b, float t) const;
  
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFNoiseGenerator);

  void Init(float seed, int steps);
  void Init(std::uint32_t seed, int steps);
  float AtScalar(float phase) const;
  FBBatch<float> AtBatch(FBBatch<float> phases) const;
  FFParkMillerPRNG& Prng() { return _prng; }
};

template <bool Smooth>
inline float
FFNoiseGenerator<Smooth>::Lerp(float lo, float hi, float t) const
{ return lo * (1 - t) + hi * t; }

template <bool Smooth>
inline float
FFNoiseGenerator<Smooth>::CosineRemap(float a, float b, float t) const
{
  FB_ASSERT(t >= 0 && t <= 1);
  return Lerp(a, b, (1 - std::cos(t * FBPi)) * 0.5f);
}

template <bool Smooth>
inline void
FFNoiseGenerator<Smooth>::InitSteps(int steps)
{
  _steps = std::clamp(steps, 2, FFNoiseGeneratorMaxSteps);
  for (int i = 0; i < _steps; ++i)
    _r[i] = _prng.NextScalar();
}

template <bool Smooth> 
inline void 
FFNoiseGenerator<Smooth>::Init(float seed, int steps)
{
  _prng = FFParkMillerPRNG(seed);
  InitSteps(steps);
}

template <bool Smooth>
inline void
FFNoiseGenerator<Smooth>::Init(std::uint32_t seed, int steps)
{
  _prng = FFParkMillerPRNG(seed);
  InitSteps(steps);
}

template <bool Smooth> 
inline float
FFNoiseGenerator<Smooth>::AtScalar(float phase) const
{
  FB_ASSERT(_steps >= 2);
  float x = phase * _steps;
  int xi = (int)x - (x < 0 && x != (int)x);
  int x_min = xi % _steps;
  if constexpr (!Smooth)
    return _r[x_min];
  else
  {
    int x_max = (x_min + 1) % _steps;
    float t = x - xi;
    return CosineRemap(_r[x_min], _r[x_max], t);
  }
}

template <bool Smooth>
inline FBBatch<float>
FFNoiseGenerator<Smooth>::AtBatch(FBBatch<float> phase) const
{
  FBSArray<float, FBSIMDFloatCount> arr;
  arr.Store(0, phase);
  for (int i = 0; i < FBSIMDFloatCount; i++)
    arr.Set(i, AtScalar(arr.Get(i)));
  return arr.Load(0);
}