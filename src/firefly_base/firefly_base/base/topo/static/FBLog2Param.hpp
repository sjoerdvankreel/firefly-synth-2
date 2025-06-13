#pragma once

#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/state/proc/FBAccParamState.hpp>
#include <firefly_base/base/topo/static/FBParamNonRealTime.hpp>

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
  double displayMultiplier = 1.0;

  float NormalizedToPlainFast(float normalized) const;
  void Init(float offset, float curveStart, float curveEnd);
  int NormalizedTimeToSamplesFast(float normalized, float sampleRate) const;
  int NormalizedFreqToSamplesFast(float normalized, float sampleRate) const;
  FBBatch<float> NormalizedToPlainFast(FBBatch<float> normalized) const;
  FBBatch<double> NormalizedToPlainFast(FBBatch<double> normalized) const;
  FBBatch<float> NormalizedToPlainFast(FBAccParamState const& normalized, int pos) const;
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
  std::string PlainToText(bool io, double plain) const override;
  std::optional<double> TextToPlain(bool io, std::string const& text) const override;
};

inline float
FBLog2Param::NormalizedToPlainFast(float normalized) const
{
  float result = _offset + _curveStart * std::pow(2.0f, _expo * normalized);
  assert(result >= _curveStart + _offset);
  return result;
}

inline FBBatch<float>
FBLog2Param::NormalizedToPlainFast(FBBatch<float> normalized) const
{
  return _offset + _curveStart * xsimd::pow(FBBatch<float>(2.0f), _expo * normalized);
}

inline FBBatch<double>
FBLog2Param::NormalizedToPlainFast(FBBatch<double> normalized) const
{
#pragma warning(push)
#pragma warning(disable: 4244)
  return _offset + _curveStart * xsimd::pow(FBBatch<double>(2.0), _expo * normalized);
#pragma warning(pop)
}

inline FBBatch<float>
FBLog2Param::NormalizedToPlainFast(FBAccParamState const& normalized, int pos) const
{
  return _offset + _curveStart * xsimd::pow(FBBatch<float>(2.0f), _expo * normalized.CV().Load(pos));
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