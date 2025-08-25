#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBMemoryPool.hpp>

#include <array>
#include <cmath>
#include <numbers>

inline float constexpr FFMaxCombFilterRes = 0.99f;
inline float constexpr FFMinCombFilterFreq = 80.0f;
inline float constexpr FFMaxCombFilterFreq = 20000.0f;

template <int Channels>
class alignas(FBSIMDAlign) FFCombFilter final
{
  float _resMin = 0.0f;
  float _resPlus = 0.0f;
  std::array<FFDelayLine<1>, Channels> _delayLinesMin = {};
  std::array<FFDelayLine<1>, Channels> _delayLinesPlus = {};

  void DebugCheck(float sampleRate, float freq);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFCombFilter);

  template <bool PlusOn, bool MinOn>
  float Next(int channel, float in);
  template <bool MinOn>
  void SetMin(float sampleRate, float freq, float res);
  template <bool PlusOn>
  void SetPlus(float sampleRate, float freq, float res);

  void Reset();
  void ReleaseBuffers(FBMemoryPool* pool);
  void AllocBuffers(FBMemoryPool* pool, float sampleRate, float minFreq);
};

template <int Channels>
inline void
FFCombFilter<Channels>::Reset()
{
  for (int c = 0; c < Channels; c++)
  {
    this->_delayLinesMin[c].Reset(this->_delayLinesMin[c].MaxBufferSize());
    this->_delayLinesPlus[c].Reset(this->_delayLinesPlus[c].MaxBufferSize());
  }
}

template <int Channels>
inline void
FFCombFilter<Channels>::ReleaseBuffers(
  FBMemoryPool* pool)
{
  for (int c = 0; c < Channels; c++)
  {
    _delayLinesMin[c].ReleaseBuffers(pool);
    _delayLinesPlus[c].ReleaseBuffers(pool);
  }
}

template <int Channels>
inline void
FFCombFilter<Channels>::AllocBuffers(
  FBMemoryPool* pool, float sampleRate, float minFreq)
{
  float maxSeconds = 1.0f / minFreq;
  float fMaxSamples = maxSeconds * sampleRate;
  int maxSamples = static_cast<int>(std::ceil(fMaxSamples));
  int const safetyCheck = 20 * 192000;
  FB_ASSERT(0 < maxSamples && maxSamples <= safetyCheck);
  for (int c = 0; c < Channels; c++)
  {
    if (_delayLinesMin[c].AllocBuffersIfChanged(pool, maxSamples))
      _delayLinesMin[c].Reset(_delayLinesMin[c].MaxBufferSize());
    if(_delayLinesPlus[c].AllocBuffersIfChanged(pool, maxSamples))
      _delayLinesPlus[c].Reset(_delayLinesPlus[c].MaxBufferSize());
  }
}

template <int Channels>
template <bool PlusOn, bool MinOn>
inline float
FFCombFilter<Channels>::Next(
  int channel, float in)
{
  FB_ASSERT(!std::isnan(in));
  FB_ASSERT(!std::isinf(in));
  
  float minOld = 0.0f;
  if constexpr (MinOn)
  {
    minOld = _delayLinesMin[channel].GetLinearInterpolate(0);
    _delayLinesMin[channel].Pop();
  }

  float plusOld = 0.0f;
  if constexpr (PlusOn)
  {
    plusOld = _delayLinesPlus[channel].GetLinearInterpolate(0);
    _delayLinesPlus[channel].Pop();
  }
  
  float out = in + _resPlus * plusOld + _resMin * minOld;  
  if constexpr (MinOn)
    _delayLinesMin[channel].Push(out);
  if constexpr (PlusOn)
    _delayLinesPlus[channel].Push(in);

  return out;
}

template <int Channels>
inline void
FFCombFilter<Channels>::DebugCheck(
  float sampleRate, float freq)
{
  (void)freq;
  (void)sampleRate;
  // check for graphs
#ifndef NDEBUG
  float nyquist = sampleRate * 0.5f;
  float minFilterFreq = FFMinCombFilterFreq;
  float maxFilterFreq = FFMaxCombFilterFreq;
  if (FFMaxCombFilterFreq > nyquist)
  {
    minFilterFreq *= nyquist / FFMaxCombFilterFreq;
    maxFilterFreq *= nyquist / FFMaxCombFilterFreq;
  }
  FB_ASSERT(minFilterFreq - 0.1f <= freq && freq <= maxFilterFreq + 0.1f);
#endif
}

template <int Channels>
template <bool MinOn>
inline void
FFCombFilter<Channels>::SetMin(
  float sampleRate, float freq, float res)
{
  if constexpr (MinOn)
  {
    DebugCheck(sampleRate, freq);
    _resMin = res * FFMaxCombFilterRes;
    for (int c = 0; c < Channels; c++)
      _delayLinesMin[c].Delay(0, sampleRate / freq);
  }
}

template <int Channels>
template <bool PlusOn>
inline void
FFCombFilter<Channels>::SetPlus(
  float sampleRate, float freq, float res)
{
  if constexpr (PlusOn)
  {
    DebugCheck(sampleRate, freq);
    _resPlus = res * FFMaxCombFilterRes;
    for (int c = 0; c < Channels; c++)
      _delayLinesPlus[c].Delay(0, sampleRate / freq);
  }
}