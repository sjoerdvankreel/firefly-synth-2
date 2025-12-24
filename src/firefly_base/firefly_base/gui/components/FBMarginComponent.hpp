#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBMarginComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
  bool const _left;
  bool const _right;
  bool const _top;
  bool const _bottom;
  bool const _opaque;

  int ExtraX() const;
  int ExtraY() const;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;

  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBMarginComponent(bool l, bool r, bool t, bool b, juce::Component* content);
  FBMarginComponent(bool l, bool r, bool t, bool b, juce::Component* content, bool opaque);
};