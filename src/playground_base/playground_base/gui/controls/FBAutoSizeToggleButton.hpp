#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBAutoSizeToggleButton:
public juce::ToggleButton,
public IFBHorizontalAutoSize
{
public:
  int FixedWidth(int height) const override;
};