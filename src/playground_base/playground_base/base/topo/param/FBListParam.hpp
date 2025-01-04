#pragma once

#include <playground_base/base/topo/param/FBListItem.hpp>

#include <string>
#include <vector>
#include <optional>
#include <algorithm>

struct FBListParam
{
  std::vector<FBListItem> items = {};

  int ValueCount() const { return (int)items.size(); }

  float PlainToNormalized(int plain) const;
  int NormalizedToPlain(float normalized) const;

  std::string PlainToText(bool io, int plain) const;
  std::optional<int> TextToPlain(bool io, std::string const& text) const;
};

inline int
FBListParam::NormalizedToPlain(float normalized) const
{
  int count = (int)items.size();
  return std::clamp((int)(normalized * count), 0, count - 1);
}