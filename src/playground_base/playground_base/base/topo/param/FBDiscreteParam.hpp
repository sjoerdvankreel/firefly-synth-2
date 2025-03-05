#pragma once

#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBDiscreteParam
{
  int valueCount = {};
  float PlainToNormalizedFast(int plain) const;
  int NormalizedToPlainFast(float normalized) const;
};

struct FBDiscreteParamNonRealTime final :
public FBDiscreteParam,
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

inline float
FBDiscreteParam::PlainToNormalizedFast(int plain) const
{
  return std::clamp(plain / (valueCount - 1.0f), 0.0f, 1.0f);
}

inline int
FBDiscreteParam::NormalizedToPlainFast(float normalized) const
{
  int scaled = static_cast<int>(normalized * valueCount);
  return std::clamp(scaled, 0, valueCount - 1);
}