#pragma once

#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>

class FBSmoothingFilter final
{
  float _lastInput = {};
  float _lastOutput = {};
  int _activeSamples = {};
  int _durationSamples = {};
  FBOnePoleFilter _onePole = {};

public:
  float RunOff();
  float Next(float in);
  void SetCoeffs(float sampleRate, float durationSecs);

  float LastOutput() const { return _lastOutput; }
  bool Active() const { return _activeSamples < _durationSamples; }
};

inline float
FBSmoothingFilter::RunOff()
{
  assert(Active());
  return Next(_lastInput);
}

inline float
FBSmoothingFilter::Next(float in)
{
  _lastInput = in;
  _activeSamples = 0;
  _lastOutput = _onePole.Next(in);
  return _lastOutput;
}

inline void
FBSmoothingFilter::SetCoeffs(float sampleRate, float durationSecs)
{
  _lastInput = 0.0f;
  _activeSamples = 0;
  _onePole.SetCoeffs(sampleRate, durationSecs);
  _durationSamples = (int)std::ceil(durationSecs * sampleRate);
}