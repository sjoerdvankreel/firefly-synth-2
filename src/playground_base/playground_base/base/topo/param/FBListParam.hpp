#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBListItem.hpp>
#include <playground_base/base/topo/param/FBItemsParamNonRealTime.hpp>

#include <vector>
#include <string>
#include <optional>
#include <algorithm>

struct FBListParamRealTime
{
  std::vector<FBListItem> items = {};
  int NormalizedToPlain(float normalized) const;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBListParamRealTime);
};

struct FBListParamNonRealTime final:
public FBListParamRealTime,
public FBItemsParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBListParamNonRealTime);
  int ValueCount() const override;
  juce::PopupMenu MakePopupMenu() const override;
  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBListParamRealTime::NormalizedToPlain(float normalized) const
{
  int count = (int)items.size();
  return std::clamp((int)std::round(normalized * count), 0, count - 1);
}