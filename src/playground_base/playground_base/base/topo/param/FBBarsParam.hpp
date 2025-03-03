#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/topo/param/FBBarsItem.hpp>
#include <playground_base/base/topo/param/FBItemsParamNonRealTime.hpp>

#include <vector>
#include <string>
#include <optional>
#include <algorithm>

std::vector<FBBarsItem>
FBMakeBarsItems(bool withZero, FBBarsItem min, FBBarsItem max);

struct FBBarsParamRealTime
{
  std::vector<FBBarsItem> items = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBarsParamRealTime);
  int NormalizedToPlain(float normalized) const;
  int NormalizedToSamples(float normalized, float sampleRate, float bpm) const;
};

struct FBBarsParamNonRealTime final :
public FBBarsParamRealTime,
public FBItemsParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBBarsParamNonRealTime);
  int ValueCount() const override;
  juce::PopupMenu MakePopupMenu() const override;
  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBBarsParamRealTime::NormalizedToPlain(float normalized) const
{
  int count = (int)items.size();
  return std::clamp((int)std::round(normalized * count), 0, count - 1);
}

inline int 
FBBarsParamRealTime::NormalizedToSamples(float normalized, float sampleRate, float bpm) const
{
  return FBBarsToSamples(items[NormalizedToPlain(normalized)], sampleRate, bpm);
}