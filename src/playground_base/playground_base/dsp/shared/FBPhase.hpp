#pragma once

#include <playground_base/dsp/shared/FBPhase.hpp>
#include <cmath>

class FBPhase
{
  float _current = 0.0f;

public:
  float Current() const { return _current; }
  float Next(float sampleRate, float frequency);
};

inline float
FBPhase::Next(float sampleRate, float frequency)
{
  float result = _current;
  _current += frequency / sampleRate;
  _current -= std::floor(_current);
  return result;
}