#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBGraphComponent final:
public juce::Component
{
public:
  void paint(juce::Graphics& g) override;
};