#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBSteppedParamNonRealTime.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBBoolParamRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBoolParamRealTime);
  bool NormalizedToPlain(float normalized) const;
};

struct FBBoolParamNonRealTime final:
public FBBoolParamRealTime,
public FBSteppedParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBoolParamNonRealTime);
  bool IsItems() const override;
  int ValueCount() const override;
  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline bool
FBBoolParamRealTime::NormalizedToPlain(float normalized) const
{
  return normalized >= 0.5f;
}