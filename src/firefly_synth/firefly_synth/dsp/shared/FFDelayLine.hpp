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
  FBDArray<float> _data = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFDelayLine);
  float Pop();
  void Reset();
  void Resize(int count);
  void Push(float val);
  void Delay(float delay);
  int Count() const { return _data.Count(); }
};

inline void
FFDelayLine::Reset()
{
  _read = 0;
  _write = 0;
  _delayWhole = 0;
  _delayFraction = 0;
  _data.SetToZero();
}

inline void
FFDelayLine::Delay(float delay)
{
  _delayWhole = static_cast<int>(delay);
  _delayFraction = delay - _delayWhole;
  assert(0.0f <= delay && delay < _data.Count());
}

inline void
FFDelayLine::Resize(int count)
{
  while (count % FBSIMDTraits<float>::Size != 0)
    count++;
  _data.Resize(count);
}

inline void
FFDelayLine::Push(float val)
{
  assert(!std::isnan(val));
  assert(!std::isinf(val));
  _data.Set(_write, val);
  _write = (_write + Count() - 1) % Count();
  assert(0 <= _write && _write < Count());
}

inline float
FFDelayLine::Pop()
{
  int pos1 = (_read + _delayWhole) % Count();
  int pos2 = (pos1 + 1) % Count();
  float val1 = _data.Get(pos1);
  float val2 = _data.Get(pos2);
  _read = (_read + Count() - 1) % Count();
  assert(0 <= _read && _read < Count());
  return val1 + _delayFraction * (val2 - val1);
}