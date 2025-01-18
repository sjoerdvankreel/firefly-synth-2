#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBGridComponent:
public juce::Component
{
public:
  juce::Grid grid = {};
  void resized() override;
  void AddItemAndChild(juce::GridItem const& item);
};