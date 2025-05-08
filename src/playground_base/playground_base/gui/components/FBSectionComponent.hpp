#pragma once

#include <playground_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBSectionComponent:
public juce::Component,
public IFBHorizontalAutoSize
{
public:
  void resized() override;
  int FixedWidth(int height) const override;
  FBSectionComponent(juce::Component* content);
};