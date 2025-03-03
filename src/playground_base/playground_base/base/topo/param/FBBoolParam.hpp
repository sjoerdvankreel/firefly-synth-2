#pragma once

#include <string>
#include <optional>

struct FBBoolParam
{
  int ValueCount() const { return 2; }
  float PlainToNormalized(bool plain) const;
  bool NormalizedToPlain(float normalized) const;
  std::string PlainToText(bool plain) const;
  std::optional<bool> TextToPlain(std::string const& text) const;
};

inline bool
FBBoolParam::NormalizedToPlain(float normalized) const
{
  return normalized >= 0.5f;
}