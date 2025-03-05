#pragma once

#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBDiscreteLog2Param
{
  int valueCount = {};
  int NormalizedToPlainFast(float normalized) const;
};

struct FBDiscreteLog2ParamNonRealTime final :
public FBDiscreteLog2Param,
public FBParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
}; 

inline int
FBDiscreteLog2Param::NormalizedToPlainFast(float normalized) const
{
  int scaled = static_cast<int>(normalized * valueCount);
  return 1 << std::clamp(scaled, 0, valueCount - 1);
}