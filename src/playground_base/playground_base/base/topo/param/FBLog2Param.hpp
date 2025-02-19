#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

// TODO do it like freqoct
struct FBLog2Param
{
  float min = 0.0f;
  float max = 1.0f;

  float PlainToNormalized(float plain) const;
  float NormalizedToPlain(float normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;
  int NormalizedTimeToSamples(float normalized, float sampleRate) const;

  int ValueCount() const { return 0; }
  std::optional<float> TextToPlain(std::string const& text) const;
  std::string PlainToText(FBValueTextDisplay display, float plain) const;
};

inline float
FBLog2Param::NormalizedToPlain(float normalized) const
{
  return min + (max - min) * std::pow(2.0f, normalized);
}

inline FBFloatVector
FBLog2Param::NormalizedToPlain(FBFloatVector normalized) const
{
  return min + (max - min) * xsimd::pow(FBFloatVector(2.0f), normalized);
}

inline FBDoubleVector
FBLog2Param::NormalizedToPlain(FBDoubleVector normalized) const
{
  return min + (max - min) * xsimd::pow(FBDoubleVector(2.0f), normalized);
}

inline int
FBLog2Param::NormalizedTimeToSamples(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlain(normalized), sampleRate);
}