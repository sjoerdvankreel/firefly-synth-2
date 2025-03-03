#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBSteppedParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBDiscreteParamRealTime
{
  int valueCount = {};
  int NormalizedToPlain(float normalized) const;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDiscreteParamRealTime);
};

struct FBDiscreteParamNonRealTime final:
public FBDiscreteParamRealTime,
public FBSteppedParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDiscreteParamNonRealTime);
  bool IsItems() const override;
  int ValueCount() const override;
  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBDiscreteParamRealTime::NormalizedToPlain(float normalized) const
{
  return std::clamp((int)std::round(normalized * valueCount), 0, valueCount - 1);
}