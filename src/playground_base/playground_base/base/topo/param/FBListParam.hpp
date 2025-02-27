#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBListItem.hpp>
#include <playground_base/base/topo/static/FBListParamNonRealTime.hpp>

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
public IFBListParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBListParamNonRealTime);
  bool IsList() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  juce::PopupMenu MakePopupMenu() const override;
  float PlainToNormalized(int plain) const override;
  int NormalizedToPlain(float normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, int plain) const override;
  std::optional<int> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBListParamRealTime::NormalizedToPlain(float normalized) const
{
  int count = (int)items.size();
  return std::clamp((int)(normalized * count), 0, count - 1);
}