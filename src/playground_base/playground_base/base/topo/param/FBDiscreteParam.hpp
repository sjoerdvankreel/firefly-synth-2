#pragma once

#include <string>
#include <optional>
#include <algorithm>
#include <functional>

struct FBDiscreteParam
{
  int valueCount = {};
  std::function<std::string(int)> toText;

  int ValueCount() const { return valueCount; }

  double PlainToNormalized(int plain) const;
  int NormalizedToPlain(double normalized) const;

  std::string PlainToText(int plain) const;
  std::optional<int> TextToPlain(std::string const& text) const;
};

inline int
FBDiscreteParam::NormalizedToPlain(double normalized) const
{
  return std::clamp((int)(normalized * valueCount), 0, valueCount - 1);
}