#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBContinuousParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBLog2ParamRealTime
{
protected:
  float _expo = {};
  float _offset = {};
  float _curveStart = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBLog2ParamRealTime);
  void Init(float offset, float curveStart, float curveEnd);

  float NormalizedToPlain(float normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;
  int NormalizedTimeToSamples(float normalized, float sampleRate) const;
  int NormalizedFreqToSamples(float normalized, float sampleRate) const;
};

struct FBLog2ParamNonRealTime final:
public FBLog2ParamRealTime,
public FBContinuousParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBLog2ParamNonRealTime);
  float PlainToNormalized(float plain) const override;
  float NormalizedToPlain(float normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, float plain) const override;
  std::optional<float> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline float
FBLog2ParamRealTime::NormalizedToPlain(float normalized) const
{
  float result = _offset + _curveStart * std::pow(2.0f, _expo * normalized);
  assert(result >= _curveStart + _offset);
  return result;
}

inline FBFloatVector 
FBLog2ParamRealTime::NormalizedToPlain(FBFloatVector normalized) const
{
  return _offset + _curveStart * xsimd::pow(FBFloatVector(2.0f), _expo * normalized);
}

inline FBDoubleVector 
FBLog2ParamRealTime::NormalizedToPlain(FBDoubleVector normalized) const
{
  return _offset + _curveStart * xsimd::pow(FBDoubleVector(2.0), _expo * normalized);
}

inline int
FBLog2ParamRealTime::NormalizedTimeToSamples(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlain(normalized), sampleRate);
}

inline int
FBLog2ParamRealTime::NormalizedFreqToSamples(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlain(normalized), sampleRate);
}