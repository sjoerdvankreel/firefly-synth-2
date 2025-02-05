#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBTextDisplay.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParam
{
  float min = 0.0f;
  float max = 1.0f;
  float displayMultiplier = 1.0f;

  int ValueCount() const { return 0; }

  std::optional<float> TextToPlain(std::string const& text) const;
  std::string PlainToText(FBTextDisplay display, float plain) const;

  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;

  float PlainToNormalized(float plain) const;
  float NormalizedToPlain(float normalized) const;
  int NormalizedTimeToSamples(float normalized, float sampleRate) const;
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

inline int
FBLinearParam::NormalizedTimeToSamples(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlain(normalized), sampleRate);
}