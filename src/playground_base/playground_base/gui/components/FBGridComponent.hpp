#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBGridComponent:
public juce::Component
{
public:
  FBGridComponent() = default;
  FBGridComponent(int rows, int cols);

  juce::Grid grid = {};
  void resized() override;
  void AddItemAndChild(juce::GridItem const& item);
};