#pragma once

#include <string>
#include <optional>

struct FBBoolParam
{
  int ValueCount() const { return 2; }

  double PlainToNormalized(bool plain) const;
  bool NormalizedToPlain(double normalized) const;

  std::string PlainToText(bool plain) const;
  std::optional<bool> TextToPlain(std::string const& text) const;
};

inline bool
FBBoolParam::NormalizedToPlain(double normalized) const
{
  return normalized >= 0.5f;
}