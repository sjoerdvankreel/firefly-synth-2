#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedDoubleBlock.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBIdentityParam
{
  double displayMultiplier = 100.0;

  float NormalizedToPlainFast(float normalized) const;
  FBFloatVector NormalizedToPlainFast(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlainFast(FBDoubleVector normalized) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatBlock& plain) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleBlock& plain) const;
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

inline FBFloatVector 
FBIdentityParam::NormalizedToPlainFast(FBFloatVector normalized) const
{
  return normalized;
}

inline FBDoubleVector 
FBIdentityParam::NormalizedToPlainFast(FBDoubleVector normalized) const
{
  return normalized; 
}

inline void 
FBIdentityParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatBlock& plain) const
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    plain[v] = NormalizedToPlainFast(normalized.CV(v));
}

inline void
FBIdentityParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleBlock& plain) const
{
  FBFixedDoubleBlock normDouble;
  normDouble.LoadCastFromFloatArray(normalized.CV());
  for (int v = 0; v < FBFixedDoubleVectors; v++)
    plain[v] = NormalizedToPlainFast(normDouble[v]);
}