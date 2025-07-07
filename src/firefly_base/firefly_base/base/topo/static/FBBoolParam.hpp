#pragma once

#include <firefly_base/base/topo/static/FBParamNonRealTime.hpp>

#include <string>
#include <optional>

struct FBBoolParam
{
  bool NormalizedToPlainFast(float normalized) const { return normalized >= 0.5f; }
}; 

struct FBBoolParamNonRealTime final:
public FBBoolParam,
public FBParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(bool io, int moduleIndex, double plain) const override;
  std::optional<double> TextToPlainInternal(bool io, int moduleIndex, std::string const& text) const override;
};