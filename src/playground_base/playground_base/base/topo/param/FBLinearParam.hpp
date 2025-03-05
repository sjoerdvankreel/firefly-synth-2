#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParam
{
  float min = 0.0f;
  float max = 1.0f;
  double editSkewFactor = 1.0f;
  double displayMultiplier = 1.0f;

  float NormalizedToPlainFast(float normalized) const;
  FBFloatVector NormalizedToPlainFast(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlainFast(FBDoubleVector normalized) const;
  int NormalizedTimeToSamplesFast(float normalized, float sampleRate) const;
  int NormalizedFreqToSamplesFast(float normalized, float sampleRate) const;
};

struct FBLinearParamNonRealTime final :
public FBLinearParam,
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

inline float
FBLinearParam::NormalizedToPlainFast(float normalized) const
{
  return min + (max - min) * normalized;
}

inline FBFloatVector 
FBLinearParam::NormalizedToPlainFast(FBFloatVector normalized) const
{
  return min + (max - min) * normalized;
}

inline FBDoubleVector 
FBLinearParam::NormalizedToPlainFast(FBDoubleVector normalized) const
{
  return min + (max - min) * normalized; 
}

inline int
FBLinearParam::NormalizedTimeToSamplesFast(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlainFast(normalized), sampleRate);
}

inline int
FBLinearParam::NormalizedFreqToSamplesFast(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlainFast(normalized), sampleRate);
}