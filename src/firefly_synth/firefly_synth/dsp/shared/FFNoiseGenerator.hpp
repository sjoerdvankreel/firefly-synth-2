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
  int _prevXMin = 0;
  bool _freeRunning = false;
  FFParkMillerPRNG _prng = {};
  std::uint32_t _prngStateAtInit = {};
  std::array<float, FFNoiseGeneratorMaxSteps> _r = {};

  void InitSteps(int steps);
  float Lerp(float lo, float hi, float t) const;
  float CosineRemap(float a, float b, float t) const;
  
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFNoiseGenerator);

  float NextScalar(float phase);
  FBBatch<float> NextBatch(FBBatch<float> phases);
  void Init(float seed, int steps, bool freeRunning);
  void Init(std::uint32_t seed, int steps, bool freeRunning);

  FFParkMillerPRNG& Prng() { return _prng; }
  std::uint32_t PrngStateAtInit() const { return _prngStateAtInit; }
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
  _prevXMin = 0;
  _steps = std::clamp(steps, 2, FFNoiseGeneratorMaxSteps);
  for (int i = 0; i < _steps; ++i)
    _r[i] = _prng.NextScalar();
}

template <bool Smooth> 
inline void 
FFNoiseGenerator<Smooth>::Init(float seed, int steps, bool freeRunning)
{
  _freeRunning = freeRunning;
  _prng = FFParkMillerPRNG(seed);
  _prngStateAtInit = _prng.State();
  InitSteps(steps);
}

template <bool Smooth>
inline void
FFNoiseGenerator<Smooth>::Init(std::uint32_t seed, int steps, bool freeRunning)
{
  _freeRunning = freeRunning;
  _prng = FFParkMillerPRNG(seed);
  _prngStateAtInit = _prng.State();
  InitSteps(steps);
}

template <bool Smooth> 
inline float
FFNoiseGenerator<Smooth>::NextScalar(float phase)
{
  FB_ASSERT(_steps >= 2);
  float x = phase * _steps;
  int xi = (int)x - (x < 0 && x != (int)x);
  int xMin = xi % _steps;
  if constexpr (!Smooth)
    return _r[xMin];
  else
  {
    if (_freeRunning && _prevXMin != xMin)
    {
      _r[_prevXMin] = _prng.NextScalar();
      _prevXMin = xMin;
    }
    int xMax = (xMin + 1) % _steps;
    float t = x - xi;
    return CosineRemap(_r[xMin], _r[xMax], t);
  }
}

template <bool Smooth>
inline FBBatch<float>
FFNoiseGenerator<Smooth>::NextBatch(FBBatch<float> phase)
{
  FBSArray<float, FBSIMDFloatCount> arr;
  arr.Store(0, phase);
  for (int i = 0; i < FBSIMDFloatCount; i++)
    arr.Set(i, NextScalar(arr.Get(i)));
  return arr.Load(0);
}