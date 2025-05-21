#pragma once

#include <playground_base/base/shared/FBSIMD.hpp>
#include <playground_base/base/shared/FBDArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>

class FBDelayLine final
{
  int _read = {};
  int _write = {};
  int _delayWhole = {};
  float _delayFraction = {};
  FBDArray<float> _data = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDelayLine);
  float Pop();
  void Push(float val);
  void Delay(float delay);
  int Count() const { return _data.Count(); }
  void Resize(int count) { _data.Resize(count); }
};

inline void
FBDelayLine::Delay(float delay)
{
  _delayWhole = static_cast<int>(delay);
  _delayFraction = delay - _delayWhole;
  assert(0.0f <= delay && delay < _data.Count());
}

inline void
FBDelayLine::Push(float val)
{
  _data.Set(_write, val);
  _write = (_write + Count() - 1) % Count();
}

inline float
FBDelayLine::Pop()
{
  int pos1 = (_read + _delayWhole) % Count();
  int pos2 = (pos1 + 1) % Count();
  float val1 = _data.Get(pos1);
  float val2 = _data.Get(pos2);
  _read = (_read + Count() - 1) % Count();
  return val1 + _delayFraction * (val2 - val1);
}