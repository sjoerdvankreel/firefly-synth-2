#pragma once

#include <playground_base/base/topo/param/FBListItem.hpp>
#include <playground_base/base/topo/param/FBItemsParamNonRealTime.hpp>

#include <string>
#include <vector>
#include <optional>
#include <algorithm>

struct FBListParam
{
  std::vector<FBListItem> items = {};
  int NormalizedToPlainFast(float normalized) const;
};

struct FBListParamNonRealTime final :
public FBListParam,
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

inline int
FBListParam::NormalizedToPlainFast(float normalized) const
{
  int count = static_cast<int>(items.size());
  return std::clamp(static_cast<int>(normalized * count), 0, count - 1);
}