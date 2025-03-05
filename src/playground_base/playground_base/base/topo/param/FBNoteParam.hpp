#pragma once

#include <playground_base/base/topo/param/FBItemsParamNonRealTime.hpp>

#include <string>
#include <optional>
#include <algorithm>

struct FBNoteParam
{
  static inline std::string const C4Name = "C4";
  static inline int constexpr MidiNoteCount = 128;
  
  int NormalizedToPlainFast(float normalized) const;
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
  std::string PlainToText(FBTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBTextDisplay display, std::string const& text) const override;
};

inline int
FBNoteParam::NormalizedToPlainFast(float normalized) const
{
  return std::clamp(static_cast<int>(normalized * MidiNoteCount), 0, MidiNoteCount - 1);
}