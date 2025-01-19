#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBSectionComponent:
public juce::Component
{
public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  FBSectionComponent(juce::Component* content);
};