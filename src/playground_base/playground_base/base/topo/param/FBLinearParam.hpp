#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParam
{
  double min = 0.0;
  double max = 1.0;
  double displayMultiplier = 1.0;

  int ValueCount() const { return 0; }

  std::string PlainToText(double plain) const;
  std::optional<double> TextToPlain(std::string const& text) const;

  double PlainToNormalized(double plain) const;
  double NormalizedToPlain(double normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;
};

inline double
FBLinearParam::NormalizedToPlain(double normalized) const
{
  return min + (max - min) * normalized;
}

inline FBFloatVector 
FBLinearParam::NormalizedToPlain(FBFloatVector normalized) const
{
  return min + (max - min) * normalized;
}

inline FBDoubleVector 
FBLinearParam::NormalizedToPlain(FBDoubleVector normalized) const
{
  return min + (max - min) * normalized; 
}