#pragma once

#include <playground_base/base/topo/param/FBBarsItem.hpp>

#include <cmath>
#include <array>
#include <numbers>
#include <cassert>
#include <algorithm>

inline constexpr int FBNoteCount = 128;
inline constexpr int FBCentCount = 100;
inline constexpr int FBNoteCentCount = FBNoteCount * FBCentCount;

inline constexpr float FBPi = std::numbers::pi_v<float>;
inline constexpr float FBTwoPi = 2.0f * FBPi;

std::array<float, FBNoteCentCount> const&
FBPitchToFreqTable();

inline int
FBFastFloor(float x)
{
  int i = (int)x;
  return i - (i > x);
}

inline void
FBPhaseWrap(float& p)
{
  if (p >= 1.0f)
    p -= FBFastFloor(p);
}

inline bool
FBPhaseWrap2(float& p)
{
  if (p < 1.0f)
    return false;
  p -= FBFastFloor(p);
  return true;
}

inline float
FBToUnipolar(float v)
{
  return (v * 0.5f) + 0.5f;
}

inline float
FBToBipolar(float v)
{
  return (v - 0.5f) * 2.0f;
}

inline int
FBTimeToSamples(float time, float sampleRate)
{
  return static_cast<int>(std::round(time * sampleRate));
}

inline int
FBFreqToSamples(float freq, float sampleRate)
{
  assert(freq > 0.0f);
  return static_cast<int>(std::round(1.0f / freq * sampleRate));
}

inline int
FBBarsToSamples(FBBarsItem const& bars, float sampleRate, float bpm)
{
  return FBTimeToSamples((bars.num * 240.0f) / (bars.denom * bpm), sampleRate);
}

inline float
FBPitchToFreqAccurateRaw(float pitch)
{
  return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f);
}

inline float
FBPitchToFreqAccurateNyquist(float pitch, float sampleRate)
{
  return std::min(sampleRate * 0.5f, FBPitchToFreqAccurateRaw(pitch));
}

inline float
FBLerp(float pos, float low, float hi)
{
  return (1.0f - pos) * low + pos * hi;
}

template <class Table>
inline float
FBLerpTable(float indexReal, int tableSize, Table const& table)
{
  float indexBounded = std::clamp(indexReal, 0.0f, static_cast<float>(tableSize - 2));
  int low = FBFastFloor(indexBounded);
  return FBLerp(indexBounded - low, table[low], table[low + 1]);
}

inline float
FBPitchToFreqFastAndInaccurate(float pitch)
{
  return FBLerpTable(pitch * 100.0f, FBNoteCentCount, FBPitchToFreqTable());
}