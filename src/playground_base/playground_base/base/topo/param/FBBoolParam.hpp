#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBParamNonRealTime.hpp>

#include <vector>
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
public IFBParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBoolParamNonRealTime);
  bool IsList() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  float PlainToNormalized(int plain) const override;
  int NormalizedToPlain(float normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, int plain) const override;
  std::optional<int> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline bool
FBBoolParamRealTime::NormalizedToPlain(float normalized) const
{
  return normalized >= 0.5f;
}