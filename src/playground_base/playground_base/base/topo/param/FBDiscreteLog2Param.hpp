#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBParamNonRealTime.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBDiscreteLog2ParamRealTime
{
  int valueCount = {};
  int NormalizedToPlain(float normalized) const;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDiscreteLog2ParamRealTime);
};

struct FBDiscreteLog2ParamNonRealTime:
public FBDiscreteLog2ParamRealTime,
public IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBDiscreteLog2ParamNonRealTime);
  int ValueCount() const override;
  float PlainToNormalized(int plain) const override;
  int NormalizedToPlain(float normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, int plain) const override;
  std::optional<int> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBDiscreteLog2ParamRealTime::NormalizedToPlain(float normalized) const
{
  return 1 << std::clamp((int)(normalized * valueCount), 0, valueCount - 1);
}