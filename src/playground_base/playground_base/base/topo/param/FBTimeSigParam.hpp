#pragma once

#include <playground_base/base/topo/param/FBTimeSigItem.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>
#include <optional>
#include <algorithm>

struct FBTimeSigParam
{
  std::vector<FBTimeSigItem> items = {};
  int ValueCount() const { return (int)items.size(); }

  juce::PopupMenu MakePopupMenu() const;
  std::string PlainToText(int plain) const;
  float PlainToNormalized(int plain) const;
  int NormalizedToPlain(float normalized) const;
  std::optional<int> TextToPlain(std::string const& text) const;
};

inline int
FBTimeSigParam::NormalizedToPlain(float normalized) const
{
  int count = (int)items.size();
  return std::clamp((int)(normalized * count), 0, count - 1);
}