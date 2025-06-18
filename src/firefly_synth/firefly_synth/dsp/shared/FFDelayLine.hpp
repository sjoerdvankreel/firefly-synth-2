#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBDArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

class FFDelayLine final
{
  int _read = {};
  int _write = {};
  int _delayWhole = {};
  float _delayFraction = {};
  int _currentBufferSize = {};
  FBDArray<float> _data = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFDelayLine);
  float Pop();
  void Push(float val);
  void Delay(float delay);
  void Reset(int currentBufferSize);
  void InitializeBuffers(int maxBufferSize);
  int MaxBufferSize() const { return _data.Count(); }
  int CurrentBufferSize() const { return _currentBufferSize; }
};

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

inline float
FFDelayLine::Pop()
{
  assert(0 < MaxBufferSize());
  assert(0 < _currentBufferSize);
  int pos1 = (_read + _delayWhole) % CurrentBufferSize();
  int pos2 = (pos1 + 1) % CurrentBufferSize();
  float val1 = _data.Get(pos1);
  float val2 = _data.Get(pos2);
  _read = (_read + CurrentBufferSize() - 1) % CurrentBufferSize();
  FB_ASSERT(0 <= _read && _read < CurrentBufferSize());
  return val1 + _delayFraction * (val2 - val1);
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