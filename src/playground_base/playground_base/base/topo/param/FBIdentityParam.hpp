#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/shared/FBFixedBlock.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBIdentityParam
{
  double displayMultiplier = 100.0;

  float NormalizedToPlainFast(float normalized) const;
  double NormalizedToPlainFast(double normalized) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatArray& plain) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleArray& plain) const;
};

struct FBIdentityParamNonRealTime final :
public FBIdentityParam,
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
FBIdentityParam::NormalizedToPlainFast(float normalized) const
{
  return normalized;
}

inline double
FBIdentityParam::NormalizedToPlainFast(double normalized) const
{
  return normalized;
}

inline void 
FBIdentityParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatArray& plain) const
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    plain[s] = NormalizedToPlainFast(normalized.CV()[s]);
}

inline void
FBIdentityParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleArray& plain) const
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    plain[s] = NormalizedToPlainFast(normalized.CV()[s]);
}