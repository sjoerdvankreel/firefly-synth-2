#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBFreqOctParam
{
  int octaves = 0;
  float minHz = 0.0f;

  int ValueCount() const { return 0; }

  std::string PlainToText(float plain) const;
  std::optional<float> TextToPlain(std::string const& text) const;

  float PlainToNormalized(float plain) const;
  float NormalizedToPlain(float normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;
};

inline float
FBFreqOctParam::NormalizedToPlain(float normalized) const
{
  return minHz * std::pow(2.0f, (float)octaves * normalized);
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