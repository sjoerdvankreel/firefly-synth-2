#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBFreqOctParam
{
  int octaves = 0;
  double minHz = 0.0;

  int ValueCount() const { return 0; }

  std::string PlainToText(double plain) const;
  std::optional<double> TextToPlain(std::string const& text) const;

  double PlainToNormalized(double plain) const;
  double NormalizedToPlain(double normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;
};

inline double
FBFreqOctParam::NormalizedToPlain(double normalized) const
{
  return minHz * std::pow(2.0, (double)octaves * normalized);
}

inline FBFloatVector 
FBFreqOctParam::NormalizedToPlain(FBFloatVector normalized) const
{
  return minHz * xsimd::pow(FBFloatVector(2.0f), (float)octaves * normalized);
}

inline FBDoubleVector 
FBFreqOctParam::NormalizedToPlain(FBDoubleVector normalized) const
{
  return minHz * xsimd::pow(FBDoubleVector(2.0), (double)octaves * normalized);
}