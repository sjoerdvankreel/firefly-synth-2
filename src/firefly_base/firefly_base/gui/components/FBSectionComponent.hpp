#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
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

class FBSubSectionComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
public:
  void resized() override;
  void paint(juce::Graphics& g) override;

  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBSubSectionComponent(juce::Component* content);
};