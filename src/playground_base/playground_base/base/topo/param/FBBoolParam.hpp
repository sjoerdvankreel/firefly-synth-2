#pragma once

#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <string>
#include <optional>

struct FBBoolParam
{
  bool NormalizedToPlain(float normalized) const;
}; 

struct FBBoolParamNonRealTime final:
public FBBoolParam,
public FBParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline bool
FBBoolParam::NormalizedToPlain(float normalized) const
{
  return normalized >= 0.5f;
}