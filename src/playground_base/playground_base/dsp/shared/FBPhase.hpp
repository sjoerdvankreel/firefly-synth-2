#pragma once

#include <cmath>

class FBPhase
{
  float _current = 0.0f;

public:
  float Current() const 
  { return _current; }
  void Next(float sampleRate, float frequency);
};

inline void
FBPhase::Next(float sampleRate, float frequency)
{
  _current += frequency / sampleRate;
  _current -= std::floor(_current);
}