#pragma once

#include <playground_plug/dsp/shared/FFDelayLine.hpp>
#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

#include <array>
#include <cmath>
#include <numbers>
#include <cassert>

template <int Channels>
class alignas(FBSIMDAlign) FFCombFilter final
{
  float _resMin = 0.0f;
  float _resPlus = 0.0f;
  std::array<FFDelayLine, Channels> _delayLinesMin = {};
  std::array<FFDelayLine, Channels> _delayLinesPlus = {};

public:
  FB_NOCOPY_MOVE_DEFCTOR(FFCombFilter);
  void Reset();
  float Next(int channel, float in);
  void Resize(float sampleRate, float minFreq);
  void Set(float sampleRate, float freqPlus, float resPlus, float freqMin, float resMin);
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
  assert(0 < maxSamples && maxSamples <= safetyCheck);
  while (maxSamples % FBSIMDTraits<float>::Size != 0)
    maxSamples++;
  for (int c = 0; c < Channels; c++)
  {
    _delayLinesMin[c].Resize(maxSamples);
    _delayLinesPlus[c].Resize(maxSamples);
  }
}

template <int Channels>
inline float
FFCombFilter<Channels>::Next(
  int channel, float in)
{
  assert(!std::isnan(in));
  assert(!std::isinf(in));
  float minOld = _delayLinesMin[channel].Pop();
  float plusOld = _delayLinesPlus[channel].Pop();
  float out = in + _resPlus * plusOld + _resMin * minOld;
  _delayLinesMin[channel].Push(out);
  _delayLinesPlus[channel].Push(in);
  return out;
}

template <int Channels>
inline void
FFCombFilter<Channels>::Set(
  float sampleRate,
  float freqPlus, float resPlus,
  float freqMin, float resMin)
{
  // need for graphs
  _resMin = resMin * FFMaxFilterRes;
  _resPlus = resPlus * FFMaxFilterRes;
  freqMin = std::clamp(freqMin, 0.0f, sampleRate * 0.5f);
  freqPlus = std::clamp(freqPlus, 0.0f, sampleRate * 0.5f);
  for (int c = 0; c < Channels; c++)
  {
    _delayLinesMin[c].Delay(sampleRate / freqMin);
    _delayLinesPlus[c].Delay(sampleRate / freqPlus);
  }
}