#pragma once

#include <firefly_synth/dsp/shared/FFDelayLine.hpp>
#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

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
  std::array<FFDelayLine, Channels> _delayLinesMin = {};
  std::array<FFDelayLine, Channels> _delayLinesPlus = {};

  void DebugCheck(float sampleRate, float freq);

public:
  FB_NOCOPY_MOVE_DEFCTOR(FFCombFilter);
  void Reset();
  void Resize(float sampleRate, float minFreq);

  template <bool PlusOn, bool MinOn>
  float Next(int channel, float in);
  template <bool MinOn>
  void SetMin(float sampleRate, float freq, float res);
  template <bool PlusOn>
  void SetPlus(float sampleRate, float freq, float res);
};

template <int Channels>
inline void
FFCombFilter<Channels>::Reset()
{
  for (int c = 0; c < Channels; c++)
  {
    this->_delayLinesMin[c].Reset();
    this->_delayLinesPlus[c].Reset();
  }
}

template <int Channels>
inline void
FFCombFilter<Channels>::Resize(
  float sampleRate, float minFreq)
{
  float maxSeconds = 1.0f / minFreq;
  float fMaxSamples = maxSeconds * sampleRate;
  int maxSamples = static_cast<int>(std::ceil(fMaxSamples));
  int const safetyCheck = 20 * 192000;
  FB_ASSERT(0 < maxSamples && maxSamples <= safetyCheck);
  for (int c = 0; c < Channels; c++)
  {
    _delayLinesMin[c].Resize(maxSamples);
    _delayLinesPlus[c].Resize(maxSamples);
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
    minOld = _delayLinesMin[channel].Pop();

  float plusOld = 0.0f;
  if constexpr (PlusOn)
    plusOld = _delayLinesPlus[channel].Pop();
  
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
      _delayLinesMin[c].Delay(sampleRate / freq);
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
      _delayLinesPlus[c].Delay(sampleRate / freq);
  }
}