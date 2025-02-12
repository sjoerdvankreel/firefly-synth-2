#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/topo/param/FBBarsItem.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>
#include <optional>
#include <algorithm>

std::vector<FBBarsItem>
FBMakeBarsItems(bool withZero, FBBarsItem min, FBBarsItem max);

struct FBBarsParam
{
  std::vector<FBBarsItem> items = {};
  int ValueCount() const { return (int)items.size(); }

  juce::PopupMenu MakePopupMenu() const;
  std::string PlainToText(int plain) const;
  std::optional<int> TextToPlain(std::string const& text) const;

  float PlainToNormalized(int plain) const;
  int NormalizedToPlain(float normalized) const;
  int NormalizedBarsToSamples(float normalized, float sampleRate, float bpm) const;
};

inline int
FBBarsParam::NormalizedToPlain(float normalized) const
{
  int count = ValueCount();
  return std::clamp((int)(normalized * count), 0, count - 1);
}

inline int 
FBBarsParam::NormalizedBarsToSamples(float normalized, float sampleRate, float bpm) const
{
  return FBBarsToSamples(items[NormalizedToPlain(normalized)], sampleRate, bpm);
}