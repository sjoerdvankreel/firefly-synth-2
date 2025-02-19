#pragma once

#include <playground_base/gui/shared/FBVerticalAutoSize.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBAutoSizeToggleButton:
public juce::ToggleButton,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};