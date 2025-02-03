#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBGUILookAndFeel:
public juce::LookAndFeel_V4
{
public:
  void drawLinearSliderOutline(
    juce::Graphics&, 
    int x, int y, int width, int height, 
    juce::Slider::SliderStyle, juce::Slider&) override;
};