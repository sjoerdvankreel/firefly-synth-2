#pragma once

#include <playground_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBAutoSizeMultiLineLabel:
public juce::Component,
public IFBHorizontalAutoSize
{
  int const _vOffset;
  std::string const _text;
  juce::Point<int> const _textSize;
public:
  void paint(juce::Graphics& g) override;
  int FixedWidth(int height) const override;
  FBAutoSizeMultiLineLabel(std::string const& text, int vOffset);
};