#pragma once

#include <playground_base/base/topo/param/FBListItem.hpp>
#include <playground_base/base/topo/static/FBListParamNonRealTime.hpp>

#include <vector>
#include <algorithm>

struct FBListParamRealTime
{
  std::vector<FBListItem> items = {};
  int NormalizedToPlain(float normalized) const;
};

struct FBListParamNonRealTime final:
public FBListParamRealTime,
public IFBListParamNonRealTime
{
  int ValueCount() const override;
  juce::PopupMenu MakePopupMenu() const override;
  float PlainToNormalized(int plain) const override;
  int NormalizedToPlain(float normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, int plain) const override;
  std::optional<int> TextToPlain(bool io, std::string const& text) const override;
};

inline int
FBListParamRealTime::NormalizedToPlain(float normalized) const
{
  int count = (int)items.size();
  return std::clamp((int)(normalized * count), 0, count - 1);
}