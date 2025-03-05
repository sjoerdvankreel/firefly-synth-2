#pragma once

#include <playground_base/base/topo/param/FBItemsParamNonRealTime.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBNoteParam
{
  static inline std::string const C4Name = "C4";
  static inline int constexpr MidiNoteCount = 128;
  
  float NormalizedToPitchFast(float normalized) const;
};

struct FBNoteParamNonRealTime final :
public FBNoteParam,
public FBItemsParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  juce::PopupMenu MakePopupMenu() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline float
FBNoteParam::NormalizedToPitchFast(float normalized) const
{
  return std::clamp(normalized * MidiNoteCount, 0.0f, MidiNoteCount - 1.0f);
}