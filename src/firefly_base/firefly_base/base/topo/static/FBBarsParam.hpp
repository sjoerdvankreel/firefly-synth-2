#pragma once

#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/static/FBBarsItem.hpp>
#include <firefly_base/base/topo/static/FBParamNonRealTime.hpp>

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
  float NormalizedToFreqFast(float normalized, float bpm) const;
  int NormalizedToSamplesFast(float normalized, float sampleRate, float bpm) const;
  float NormalizedToFloatSamplesFast(float normalized, float sampleRate, float bpm) const;
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
  juce::PopupMenu MakePopupMenu(int moduleIndex) const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(bool io, int moduleIndex, double plain) const override;
  std::optional<double> TextToPlainInternal(bool io, int moduleIndex, std::string const& text) const override;
};

inline int
FBBarsParam::NormalizedToPlainFast(float normalized) const
{
  int count = static_cast<int>(items.size());
  return std::clamp(static_cast<int>(normalized * count), 0, count - 1);
}

inline float 
FBBarsParam::NormalizedToFreqFast(float normalized, float bpm) const
{
  return FBBarsToFreq(items[NormalizedToPlainFast(normalized)], bpm);
}

inline int 
FBBarsParam::NormalizedToSamplesFast(float normalized, float sampleRate, float bpm) const
{
  return FBBarsToSamples(items[NormalizedToPlainFast(normalized)], sampleRate, bpm);
}

inline float
FBBarsParam::NormalizedToFloatSamplesFast(float normalized, float sampleRate, float bpm) const
{
  return FBBarsToFloatSamples(items[NormalizedToPlainFast(normalized)], sampleRate, bpm);
}