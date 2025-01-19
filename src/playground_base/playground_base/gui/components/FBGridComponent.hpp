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

  void Add(juce::Component* child);
  void Add(juce::GridItem const& item);
};