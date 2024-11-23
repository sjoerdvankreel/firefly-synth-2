#pragma once

#include <cmath>

class FFPhase
{
  float _current = 0.0f;

public:
  float Current() const 
  { return _current; }
  void Next(float sampleRate, float frequency);
};

inline void
FFPhase::Next(float sampleRate, float frequency)
{
  _current += frequency / sampleRate;
  _current -= std::floor(_current);
}