#pragma once

#include <playground_base/base/shared/FBVector.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParam
{
  float min = 0.0f;
  float max = 1.0f;
  float displayMultiplier = 1.0f;

  std::string PlainToText(float plain) const;
  std::optional<float> TextToPlain(std::string const& text) const;

  float PlainToNormalized(float plain) const;
  float NormalizedToPlain(float normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;
};

inline float
FBLinearParam::NormalizedToPlain(float normalized) const
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