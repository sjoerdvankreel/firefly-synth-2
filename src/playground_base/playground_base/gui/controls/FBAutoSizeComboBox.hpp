#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBAutoSizeComboBox:
public juce::ComboBox,
public IFBHorizontalAutoSize
{
  int _maxTextWidth = 0;

public:
  int FixedWidth(int height) const override;
  FBAutoSizeComboBox(juce::PopupMenu const& rootMenu);
};