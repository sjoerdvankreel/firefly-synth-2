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

  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;

  float PlainToNormalized(float plain) const;
  float NormalizedToPlain(float normalized) const;
  int NormalizedTimeToSamples(float normalized, float sampleRate) const;
  int NormalizedFreqToSamples(float normalized, float sampleRate) const;

  std::optional<float> TextToPlain(std::string const& text) const;
  std::string PlainToText(FBValueTextDisplay display, float plain) const;
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

inline int
FBLinearParam::NormalizedFreqToSamples(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlain(normalized), sampleRate);
}