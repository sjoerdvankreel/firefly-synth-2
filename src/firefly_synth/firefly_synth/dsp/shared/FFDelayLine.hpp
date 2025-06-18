#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBDArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

// Stolen from the JUCE version, 
// just rearranged a bit cause we don't need the channel dimension.
class FFDelayLine final
{
  int _read = {};
  int _write = {};
  int _delayWhole = {};
  float _delayFraction = {};
  int _currentBufferSize = {};
  FBDArray<float> _data = {};

  template <bool LagrangeInterpolation>
  float InterpolateCurrent();

public:

  FB_NOCOPY_NOMOVE_DEFCTOR(FFDelayLine);
  template <bool LagrangeInterpolation>
  float Pop();

  void Push(float val);
  void Delay(float delay);
  void Reset(int currentBufferSize);
  void InitializeBuffers(int maxBufferSize);
  int MaxBufferSize() const { return _data.Count(); }
  int CurrentBufferSize() const { return _currentBufferSize; }
};

template <bool LagrangeInterpolation>
inline float
FFDelayLine::InterpolateCurrent()
{
  if constexpr (LagrangeInterpolation)
  {
    int pos1 = _read + _delayWhole;
    int pos2 = pos1 + 1;
    int pos3 = pos1 + 2;
    int pos4 = pos1 + 3;
    if (pos4 >= CurrentBufferSize())
    {
      pos1 %= CurrentBufferSize();
      pos2 %= CurrentBufferSize();
      pos3 %= CurrentBufferSize();
      pos4 %= CurrentBufferSize();
    }

    float val1 = _data.Get(pos1);
    float val2 = _data.Get(pos2);
    float val3 = _data.Get(pos3);
    float val4 = _data.Get(pos4);
    float d1 = _delayFraction - 1.0f;
    float d2 = _delayFraction - 2.0f;
    float d3 = _delayFraction - 3.0f;
    float c1 = -d1 * d2 * d3 / 6.0f;
    float c2 = d2 * d3 * 0.5f;
    float c3 = -d1 * d3 * 0.5f;
    float c4 = d1 * d2 / 6.0f;
    return val1 * c1 + _delayFraction * (val2 * c2 + val3 * c3 + val4 * c4);
  }
  else
  {
    int pos1 = (_read + _delayWhole) % CurrentBufferSize();
    int pos2 = (pos1 + 1) % CurrentBufferSize();
    float val1 = _data.Get(pos1);
    float val2 = _data.Get(pos2);
    return val1 + _delayFraction * (val2 - val1);
  }
}

inline void
FFDelayLine::InitializeBuffers(int maxBufferSize)
{
  while (maxBufferSize % FBSIMDTraits<float>::Size != 0)
    maxBufferSize++;
  _data.Resize(maxBufferSize);
  _currentBufferSize = 0;
}

inline void
FFDelayLine::Delay(float delay)
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  _delayWhole = static_cast<int>(delay);
  _delayFraction = delay - _delayWhole;
  FB_ASSERT(0.0f <= delay && delay < CurrentBufferSize());
}

inline void
FFDelayLine::Push(float val)
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  FB_ASSERT(!std::isnan(val));
  FB_ASSERT(!std::isinf(val));
  _data.Set(_write, val);
  _write = (_write + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _write && _write < CurrentBufferSize());
}

template <bool LagrangeInterpolation>
inline float
FFDelayLine::Pop()
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  float result = InterpolateCurrent<LagrangeInterpolation>();
  _read = (_read + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _read && _read < CurrentBufferSize());
  return result;
}

inline void
FFDelayLine::Reset(int currentBufferSize)
{
  _read = 0;
  _write = 0;
  _delayWhole = 0;
  _delayFraction = 0;
  _data.SetToZero();

  assert(0 < MaxBufferSize());
  while (currentBufferSize % FBSIMDTraits<float>::Size != 0)
    currentBufferSize++;
  FB_ASSERT(0 <= currentBufferSize && currentBufferSize <= _data.Count());
  _currentBufferSize = currentBufferSize;
}