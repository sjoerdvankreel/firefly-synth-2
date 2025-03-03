#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBContinuousParamNonRealTime.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParamRealTime
{
  float min = 0.0f;
  float max = 1.0f;
  float editSkewFactor = 1.0f;
  float displayMultiplier = 1.0f;

  FB_NOCOPY_NOMOVE_DEFCTOR(FBLinearParamRealTime);
  float NormalizedToPlain(float normalized) const;
  FBFloatVector NormalizedToPlain(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlain(FBDoubleVector normalized) const;
  int NormalizedTimeToSamples(float normalized, float sampleRate) const;
  int NormalizedFreqToSamples(float normalized, float sampleRate) const;
};

struct FBLinearParamNonRealTime final:
public FBLinearParamRealTime,
public FBContinuousParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBLinearParamNonRealTime);
  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline float
FBLinearParamRealTime::NormalizedToPlain(float normalized) const
{
  return min + (max - min) * normalized;
}

inline FBFloatVector 
FBLinearParamRealTime::NormalizedToPlain(FBFloatVector normalized) const
{
  return min + (max - min) * normalized;
}

inline FBDoubleVector 
FBLinearParamRealTime::NormalizedToPlain(FBDoubleVector normalized) const
{
  return min + (max - min) * normalized; 
}

inline int
FBLinearParamRealTime::NormalizedTimeToSamples(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlain(normalized), sampleRate);
}

inline int
FBLinearParamRealTime::NormalizedFreqToSamples(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlain(normalized), sampleRate);
}