#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParam
{
  float min = 0.0f;
  float max = 1.0f;
  double editSkewFactor = 1.0;
  double displayMultiplier = 1.0;

  float NormalizedToPlainFast(float normalized) const;
  int NormalizedTimeToSamplesFast(float normalized, float sampleRate) const;
  int NormalizedFreqToSamplesFast(float normalized, float sampleRate) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatArray& plain) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleArray& plain) const;
};

struct FBLinearParamNonRealTime final :
public FBLinearParam,
public FBParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBTextDisplay display, std::string const& text) const override;
};

inline float
FBLinearParam::NormalizedToPlainFast(float normalized) const
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

inline void 
FBLinearParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatArray& plain) const
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    plain[s] = NormalizedToPlainFast(normalized.CV()[s]);
}

inline void
FBLinearParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleArray& plain) const
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    plain[s] = NormalizedToPlainFast(normalized.CV()[s]);
}