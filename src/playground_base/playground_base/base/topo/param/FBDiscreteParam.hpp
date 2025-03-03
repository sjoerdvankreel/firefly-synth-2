#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBParamNonRealTime.hpp>

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
public IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDiscreteParamNonRealTime);
  bool IsList() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  float PlainToNormalized(int plain) const override;
  int NormalizedToPlain(float normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, int plain) const override;
  std::optional<int> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBDiscreteParamRealTime::NormalizedToPlain(float normalized) const
{
  return std::clamp((int)(normalized * valueCount), 0, valueCount - 1);
}