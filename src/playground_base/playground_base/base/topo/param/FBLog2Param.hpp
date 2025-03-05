#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBLog2Param
{
protected:
  float _expo = {};
  float _offset = {};
  float _curveStart = {};

public:
  float NormalizedToPlainFast(float normalized) const;
  void Init(float offset, float curveStart, float curveEnd);
  FBFloatVector NormalizedToPlainFast(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlainFast(FBDoubleVector normalized) const;
  int NormalizedTimeToSamplesFast(float normalized, float sampleRate) const;
  int NormalizedFreqToSamplesFast(float normalized, float sampleRate) const;
};

struct FBLog2ParamNonRealTime final :
public FBLog2Param,
public FBParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBTextDisplay display, std::string const& text) const override;
};

inline float
FBLog2Param::NormalizedToPlainFast(float normalized) const
{
  float result = _offset + _curveStart * std::pow(2.0f, _expo * normalized);
  assert(result >= _curveStart + _offset);
  return result;
}

inline FBFloatVector 
FBLog2Param::NormalizedToPlainFast(FBFloatVector normalized) const
{
  return _offset + _curveStart * xsimd::pow(FBFloatVector(2.0f), _expo * normalized);
}

inline FBDoubleVector 
FBLog2Param::NormalizedToPlainFast(FBDoubleVector normalized) const
{
#pragma warning(push)
#pragma warning(disable : 4244)
  return _offset + _curveStart * xsimd::pow(FBDoubleVector(2.0), _expo * normalized);
#pragma warning(pop)
}

inline int
FBLog2Param::NormalizedTimeToSamplesFast(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlainFast(normalized), sampleRate);
}

inline int
FBLog2Param::NormalizedFreqToSamplesFast(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlainFast(normalized), sampleRate);
}