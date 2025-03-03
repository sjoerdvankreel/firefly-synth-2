#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <optional>
#include <algorithm>

struct FBNoteParam
{
  static inline std::string const C4Name = "C4";
  static inline int constexpr MidiNoteCount = 128;
  int ValueCount() const { return MidiNoteCount; }

  juce::PopupMenu MakePopupMenu() const;
  float PlainToNormalized(int plain) const;
  std::string PlainToText(int plain) const;
  int NormalizedToPlain(float normalized) const;
  std::optional<int> TextToPlain(std::string const& text) const;
};

inline int
FBNoteParam::NormalizedToPlain(float normalized) const
{
  return std::clamp((int)(normalized * ValueCount()), 0, ValueCount() - 1);
}