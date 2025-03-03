#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBSteppedParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBDiscreteLog2ParamRealTime
{
  int valueCount = {};
  int NormalizedToPlain(float normalized) const;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDiscreteLog2ParamRealTime);
};

struct FBDiscreteLog2ParamNonRealTime final:
public FBDiscreteLog2ParamRealTime,
public FBSteppedParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDiscreteLog2ParamNonRealTime);
  bool IsItems() const override;
  int ValueCount() const override;
  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBDiscreteLog2ParamRealTime::NormalizedToPlain(float normalized) const
{
  return 1 << std::clamp((int)std::round(normalized * valueCount), 0, valueCount - 1);
}