#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBContentComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
public:
  void resized() override;
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  void SetContent(juce::Component* content);
};