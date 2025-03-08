#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBAutoSizeSlider:
public juce::Slider,
public IFBHorizontalAutoSize
{
protected:
  FBAutoSizeSlider(FBPlugGUI* plugGUI, juce::Slider::SliderStyle style);
public:
  int FixedWidth(int height) const override;
};