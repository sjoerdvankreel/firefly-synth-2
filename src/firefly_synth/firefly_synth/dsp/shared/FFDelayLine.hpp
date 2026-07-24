#pragma once

#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/shared/FBMemoryPool.hpp>

#include <array>
#include <cmath>
#include <limits>
#include <algorithm>

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
  // Guard against a non-finite delay value reaching the float-to-int cast
  // below -- undefined behavior otherwise (e.g. an inf from a tempo-sync
  // divide-by-zero elsewhere deterministically casts to INT_MIN on x86 in
  // practice), and this is what used to feed a corrupted, deeply-negative
  // index into GetLagrangeInterpolate/GetLinearInterpolate.
  //
  // Deliberately NOT clamping to CurrentBufferSize() here: a legitimate
  // delay value larger than the current buffer (e.g. a low note's
  // Karplus-Strong period near FFOsciStringMinFreq) is meant to wrap
  // around via the modulo in GetLinearInterpolate/GetLagrangeInterpolate,
  // not saturate. An earlier version of this fix clamped to
  // CurrentBufferSize()-1, which collapsed every delay value above that
  // ceiling to the same value -- turning many different (mostly low)
  // notes into the exact same wrong pitch. Only clamp to the range a
  // float-to-int cast can represent safely; that's astronomically larger
  // than any real delay value, so it only ever engages for genuinely
  // corrupted (inf-derived) input, never for legitimate long periods.
  if (!std::isfinite(delay))
    delay = 0.0f;
  delay = std::clamp(delay, (float)std::numeric_limits<int>::min(), (float)std::numeric_limits<int>::max());
  _delayWhole[tap] = static_cast<int>(delay);
  _delayFraction[tap] = delay - _delayWhole[tap];
  FB_ASSERT(0.0f <= delay);
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
  // Double-modulo: C++'s % can return a negative result when the left
  // operand is negative, which _delayWhole[tap] can be if a corrupted
  // (e.g. non-finite-derived) delay value ever reaches here despite the
  // guard in Delay() -- this keeps pos1/pos2 in [0, CurrentBufferSize())
  // unconditionally instead of risking an out-of-bounds _data[] read.
  int size = CurrentBufferSize();
  int pos1 = ((_read + _delayWhole[tap]) % size + size) % size;
  int pos2 = (pos1 + 1) % size;
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
  // Previously only wrapped when pos4 >= _currentBufferSize, which
  // guards the too-large-positive case but does nothing for a negative
  // pos1 (e.g. from a corrupted _delayWhole[tap]) -- that fell straight
  // through to an out-of-bounds _data[pos1] read below. Always wrap via
  // double-modulo instead, which is safe for any pos1 regardless of sign.
  if (pos1 < 0 || pos4 >= _currentBufferSize)
  {
    pos1 = ((pos1 % _currentBufferSize) + _currentBufferSize) % _currentBufferSize;
    pos2 = ((pos2 % _currentBufferSize) + _currentBufferSize) % _currentBufferSize;
    pos3 = ((pos3 % _currentBufferSize) + _currentBufferSize) % _currentBufferSize;
    pos4 = ((pos4 % _currentBufferSize) + _currentBufferSize) % _currentBufferSize;
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