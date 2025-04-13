#pragma once

#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <playground_base/base/topo/param/FBItemsParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>
#include <functional>

typedef std::function<std::string(int val)>
FBParamValueFormatter;

struct FBDiscreteParam
{
  int valueCount = {};
  int valueOffset = {};
  FBParamValueFormatter valueFormatter = {};
  float PlainToNormalizedFast(int plain) const;
  int NormalizedToPlainFast(float normalized) const;
};

struct FBDiscreteParamNonRealTime final :
public FBDiscreteParam,
public FBItemsParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;
  juce::PopupMenu MakePopupMenu() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBTextDisplay display, std::string const& text) const override;
};

inline float
FBDiscreteParam::PlainToNormalizedFast(int plain) const
{
  return std::clamp((plain - valueOffset) / (valueCount - 1.0f), 0.0f, 1.0f);
}

inline int
FBDiscreteParam::NormalizedToPlainFast(float normalized) const
{
  int scaled = static_cast<int>(normalized * valueCount);
  return std::clamp(scaled, 0, valueCount - 1) + valueOffset;
}