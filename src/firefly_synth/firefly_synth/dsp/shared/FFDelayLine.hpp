#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBMemoryPool.hpp>

#include <array>

class FBMemoryPool;

// Rip-off from JUCE, just tailored a bit.
template <int TapCount>
class FFDelayLine final
{
  int _read = {};
  int _write = {};
  float* _data = {};
  int _maxBufferSize = {};
  int _currentBufferSize = {};
  std::array<int, TapCount> _delayWhole = {};
  std::array<float, TapCount> _delayFraction = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFDelayLine);

  void Pop();
  void Push(float val);
  void Delay(int tap, float delay);
  void Reset(int currentBufferSize);

  void ReleaseBuffers(FBMemoryPool* pool);
  bool AllocBuffersIfChanged(FBMemoryPool* pool, int maxBufferSize);

  float GetLinearInterpolate(int tap);
  float GetLagrangeInterpolate(int tap);
  int MaxBufferSize() const { return _maxBufferSize; }
  int CurrentBufferSize() const { return _currentBufferSize; }
};

template <int TapCount>
inline void
FFDelayLine<TapCount>::ReleaseBuffers(FBMemoryPool* pool)
{
  _maxBufferSize = 0;
  if (_data != nullptr)
    pool->Return(_data);
  _data = nullptr;
}

template <int TapCount>
inline bool
FFDelayLine<TapCount>::AllocBuffersIfChanged(FBMemoryPool* pool, int maxBufferSize)
{
  if (_maxBufferSize == maxBufferSize)
    return false;
  if (_data != nullptr)
    pool->Return(_data);
  _data = static_cast<float*>(pool->Lease(maxBufferSize * sizeof(float)));
  _maxBufferSize = maxBufferSize;
  _currentBufferSize = 0;
  return true;
}

template <int TapCount>
inline void
FFDelayLine<TapCount>::Delay(int tap, float delay)
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  _delayWhole[tap] = static_cast<int>(delay);
  _delayFraction[tap] = delay - _delayWhole[tap];
  FB_ASSERT(0.0f <= delay && delay < CurrentBufferSize());
}

template <int TapCount>
inline void
FFDelayLine<TapCount>::Push(float val)
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  FB_ASSERT(!std::isnan(val));
  FB_ASSERT(!std::isinf(val));
  _data[_write] = val;
  _write = (_write + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _write && _write < CurrentBufferSize());
}

template <int TapCount>
inline void
FFDelayLine<TapCount>::Pop()
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  _read = (_read + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _read && _read < CurrentBufferSize());
}

template <int TapCount>
inline float
FFDelayLine<TapCount>::GetLinearInterpolate(int tap)
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  int pos1 = (_read + _delayWhole[tap]) % CurrentBufferSize();
  int pos2 = (pos1 + 1) % CurrentBufferSize();
  float val1 = _data[pos1];
  float val2 = _data[pos2];
  return val1 + _delayFraction[tap] * (val2 - val1);
}

template <int TapCount>
inline float
FFDelayLine<TapCount>::GetLagrangeInterpolate(int tap)
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  int pos1 = (_read + _delayWhole[tap]);
  int pos2 = pos1 + 1;
  int pos3 = pos1 + 2;
  int pos4 = pos1 + 3;
  if (pos4 >= _currentBufferSize)
  {
    pos1 %= _currentBufferSize;
    pos2 %= _currentBufferSize;
    pos3 %= _currentBufferSize;
    pos4 %= _currentBufferSize;
  }
  float val1 = _data[pos1];
  float val2 = _data[pos2];
  float val3 = _data[pos3];
  float val4 = _data[pos4];
  float d1 = _delayFraction[tap] - 1.0f;
  float d2 = _delayFraction[tap] - 2.0f;
  float d3 = _delayFraction[tap] - 3.0f;
  float c1 = -d1 * d2 * d3 / 6.0f;
  float c2 = d2 * d3 * 0.5f;
  float c3 = -d1 * d3 * 0.5f;
  float c4 = d1 * d2 / 6.0f;
  return val1 * c1 + _delayFraction[tap] * (val2 * c2 + val3 * c3 + val4 * c4);
}

template <int TapCount>
inline void
FFDelayLine<TapCount>::Reset(int currentBufferSize)
{
  _read = 0;
  _write = 0;
  _delayWhole = {};
  _delayFraction = {};
  std::fill(_data, _data + _maxBufferSize, 0.0f);
  FB_ASSERT(0 <= currentBufferSize && currentBufferSize <= _maxBufferSize);
  _currentBufferSize = currentBufferSize;
}