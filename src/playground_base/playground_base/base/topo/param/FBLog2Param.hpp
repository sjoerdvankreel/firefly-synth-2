#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBLog2Param
{
private:
  float _expo = {};
  float _offset = {};
  float _curveStart = {};

public:
  void Init(float offset, float curveStart, float curveEnd);

  float PlainToNormalized(float plain) const;
  float NormalizedToPlain(float normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;

  int NormalizedTimeToSamples(float normalized, float sampleRate) const;
  int NormalizedFreqToSamples(float normalized, float sampleRate) const;

  int ValueCount() const { return 0; }
  std::optional<float> TextToPlain(std::string const& text) const;
  std::string PlainToText(FBValueTextDisplay display, float plain) const;
};

inline float
FBLog2Param::NormalizedToPlain(float normalized) const
{
  float result = _offset + _curveStart * std::pow(2.0f, _expo * normalized);
  assert(result >= _curveStart + _offset);
  return result;
}

inline FBFloatVector 
FBLog2Param::NormalizedToPlain(FBFloatVector normalized) const
{
  return _offset + _curveStart * xsimd::pow(FBFloatVector(2.0f), _expo * normalized);
}

inline FBDoubleVector 
FBLog2Param::NormalizedToPlain(FBDoubleVector normalized) const
{
  return _offset + _curveStart * xsimd::pow(FBDoubleVector(2.0), _expo * normalized);
}

inline int
FBLog2Param::NormalizedTimeToSamples(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlain(normalized), sampleRate);
}

inline int
FBLog2Param::NormalizedFreqToSamples(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlain(normalized), sampleRate);
}