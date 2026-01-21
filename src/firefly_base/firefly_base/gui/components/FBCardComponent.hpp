#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <chrono>

class FBPlugGUI;

class FBCardComponent :
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
  FBPlugGUI* const _plugGUI;
  juce::Component* const _content;

public: 
  FBCardComponent(FBPlugGUI* plugGUI, juce::Component* content);
  
  void resized() override;
  void paint(juce::Graphics& g) override;
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};