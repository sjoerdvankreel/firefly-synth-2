#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/topo/static/FBBarsItem.hpp>
#include <playground_base/base/topo/static/FBParamNonRealTime.hpp>

#include <string>
#include <vector>
#include <optional>
#include <algorithm>

std::vector<FBBarsItem>
FBMakeBarsItems(bool withZero, FBBarsItem min, FBBarsItem max); 

struct FBBarsParam
{
  std::vector<FBBarsItem> items = {};
  int NormalizedToPlainFast(float normalized) const;
  int NormalizedToSamplesFast(float normalized, float sampleRate, float bpm) const;
};

struct FBBarsParamNonRealTime final :
public FBBarsParam,
public FBItemsParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;
  juce::PopupMenu MakePopupMenu() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(bool io, double plain) const override;
  std::optional<double> TextToPlain(bool io, std::string const& text) const override;
};

inline int
FBBarsParam::NormalizedToPlainFast(float normalized) const
{
  int count = static_cast<int>(items.size());
  return std::clamp(static_cast<int>(normalized * count), 0, count - 1);
}

inline int 
FBBarsParam::NormalizedToSamplesFast(float normalized, float sampleRate, float bpm) const
{
  return FBBarsToSamples(items[NormalizedToPlainFast(normalized)], sampleRate, bpm);
}