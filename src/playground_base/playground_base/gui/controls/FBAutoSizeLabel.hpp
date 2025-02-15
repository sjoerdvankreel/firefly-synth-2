#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBAutoSizeLabel:
public juce::Label,
public IFBHorizontalAutoSize
{
  int const _textWidth;
public:
  FBAutoSizeLabel(std::string const& text);
  int FixedWidth(int height) const override;
};