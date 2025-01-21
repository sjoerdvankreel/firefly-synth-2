#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBSectionComponent:
public juce::Component,
public IFBHorizontalAutoSize
{
public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  int FixedWidth(int height) const override;
  FBSectionComponent(FBPlugGUI* plugGUI, juce::Component* content);
};