#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBMemoryPool.hpp>

class FBMemoryPool;

// Rip-off from JUCE, just tailored a bit.
class FFDelayLine final
{
  int _read = {};
  int _write = {};
  int _delayWhole = {};
  float _delayFraction = {};

  float* _data = {};
  int _maxBufferSize = {};
  int _currentBufferSize = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFDelayLine);

  void Push(float val);
  void Delay(float delay);
  void Reset(int currentBufferSize);

  void ReleaseBuffers(FBMemoryPool* pool);
  void InitBuffers(FBMemoryPool* pool, int maxBufferSize);

  float PopLinearInterpolate();
  float PopLagrangeInterpolate();
  int MaxBufferSize() const { return _maxBufferSize; }
  int CurrentBufferSize() const { return _currentBufferSize; }
};

inline void
FFDelayLine::ReleaseBuffers(FBMemoryPool* pool)
{
  _maxBufferSize = 0;
  if (_data != nullptr)
    pool->Return(_data);
  _data = nullptr;
}

inline void
FFDelayLine::InitBuffers(FBMemoryPool* pool, int maxBufferSize)
{
  if (_maxBufferSize == maxBufferSize)
    return;
  if (_data != nullptr)
    pool->Return(_data);
  _data = static_cast<float*>(pool->Lease(maxBufferSize * sizeof(float)));
  _maxBufferSize = maxBufferSize;
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
  _data[_write] = val;
  _write = (_write + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _write && _write < CurrentBufferSize());
}

inline float
FFDelayLine::PopLinearInterpolate()
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  int pos1 = (_read + _delayWhole) % CurrentBufferSize();
  int pos2 = (pos1 + 1) % CurrentBufferSize();
  float val1 = _data[pos1];
  float val2 = _data[pos2];
  _read = (_read + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _read && _read < CurrentBufferSize());
  return val1 + _delayFraction * (val2 - val1);
}

inline float
FFDelayLine::PopLagrangeInterpolate()
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  int pos1 = (_read + _delayWhole);
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
  float d1 = _delayFraction - 1.0f;
  float d2 = _delayFraction - 2.0f;
  float d3 = _delayFraction - 3.0f;
  float c1 = -d1 * d2 * d3 / 6.0f;
  float c2 = d2 * d3 * 0.5f;
  float c3 = -d1 * d3 * 0.5f;
  float c4 = d1 * d2 / 6.0f;
  _read = (_read + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _read && _read < CurrentBufferSize());
  return val1 * c1 + _delayFraction * (val2 * c2 + val3 * c3 + val4 * c4);
}

inline void
FFDelayLine::Reset(int currentBufferSize)
{
  _read = 0;
  _write = 0;
  _delayWhole = 0;
  _delayFraction = 0;
  std::fill(_data, _data + _maxBufferSize, 0.0f);

  assert(0 < MaxBufferSize());
  assert(0 < currentBufferSize);
  while (currentBufferSize % FBSIMDTraits<float>::Size != 0)
    currentBufferSize++;
  FB_ASSERT(0 <= currentBufferSize && currentBufferSize <= _maxBufferSize);
  _currentBufferSize = currentBufferSize;
}