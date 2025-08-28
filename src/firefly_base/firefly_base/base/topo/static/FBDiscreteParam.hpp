#pragma once

#include <firefly_base/base/topo/static/FBParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>
#include <functional>

typedef std::function<std::string(int val)>
FBParamValueFormatter;
typedef std::function<std::string(int subMenu)>
FBParamSubMenuFormatter;

struct FBDiscreteParam
{
  int valueCount = {};
  int valueOffset = {};
  int subMenuItemCount = {};  
  FBParamValueFormatter valueFormatter = {};
  FBParamSubMenuFormatter subMenuFormatter = {};
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
  int ValueOffset() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;
  juce::PopupMenu MakePopupMenu(int moduleIndex) const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(bool io, int moduleIndex, double plain) const override;
  std::optional<double> TextToPlainInternal(bool io, int moduleIndex, std::string const& text) const override;
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