#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>
#include <playground_base/base/topo/static/FBParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBIdentityParam
{
  double displayMultiplier = 100.0;

  float NormalizedToPlainFast(float normalized) const {  return normalized; }
  double NormalizedToPlainFast(double normalized) const {  return normalized; }
  FBBatch<float> NormalizedToPlainFast(FBBatch<float> normalized) const { return normalized; }
  FBBatch<double> NormalizedToPlainFast(FBBatch<double> normalized) const { return normalized; }
  FBBatch<float> NormalizedToPlainFast(FBAccParamState const& normalized, int pos) const;
};

struct FBIdentityParamNonRealTime final :
public FBIdentityParam,
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

inline FBBatch<float>
FBIdentityParam::NormalizedToPlainFast(FBAccParamState const& normalized, int pos) const
{
  return normalized.CV().Load(pos);
}