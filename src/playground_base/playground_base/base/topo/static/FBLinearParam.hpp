#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>
#include <playground_base/base/topo/static/FBParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParam
{
  float min = 0.0f;
  float max = 1.0f;
  double editSkewFactor = 1.0;
  double displayMultiplier = 1.0;

  float NormalizedToPlainFast(float normalized) const;
  int NormalizedTimeToSamplesFast(float normalized, float sampleRate) const;
  int NormalizedFreqToSamplesFast(float normalized, float sampleRate) const;
  FBBatch<float> NormalizedToPlainFast(FBBatch<float> normalized) const;
  FBBatch<double> NormalizedToPlainFast(FBBatch<double> normalized) const;
  FBBatch<float> NormalizedToPlainFast(FBAccParamState const& normalized, int pos) const;
};

struct FBLinearParamNonRealTime final :
public FBLinearParam,
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
FBLinearParam::NormalizedToPlainFast(float normalized) const
{
  return min + (max - min) * normalized;
}

inline FBBatch<float>
FBLinearParam::NormalizedToPlainFast(FBBatch<float> normalized) const
{
  return min + (max - min) * normalized;
}

inline FBBatch<double>
FBLinearParam::NormalizedToPlainFast(FBBatch<double> normalized) const
{
  return min + (max - min) * normalized;
}

inline FBBatch<float>
FBLinearParam::NormalizedToPlainFast(FBAccParamState const& normalized, int pos) const
{
  return min + (max - min) * normalized.CV().Load(pos);
}

inline int
FBLinearParam::NormalizedTimeToSamplesFast(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlainFast(normalized), sampleRate);
}

inline int
FBLinearParam::NormalizedFreqToSamplesFast(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlainFast(normalized), sampleRate);
}