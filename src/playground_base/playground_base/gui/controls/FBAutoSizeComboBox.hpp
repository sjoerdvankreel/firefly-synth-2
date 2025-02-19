#pragma once

#include <playground_base/gui/shared/FBVerticalAutoSize.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBAutoSizeComboBox:
public juce::ComboBox,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int _maxTextWidth = 0;

public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBAutoSizeComboBox(juce::PopupMenu const& rootMenu);
};