#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBAutoSizeMultiLineLabel:
public juce::Component,
public IFBHorizontalAutoSize
{
  int const _textWidth;
  std::string const _text;
public:
  void paint(juce::Graphics& g) override;
  int FixedWidth(int height) const override;
  FBAutoSizeMultiLineLabel(std::string const& text);
};