#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBSectionComponent:
public juce::Component
{
public:
  void resized() override;
  FBSectionComponent(juce::Component* content);
};