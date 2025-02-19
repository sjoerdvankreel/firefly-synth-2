#pragma once

#include <playground_base/gui/shared/FBVerticalAutoSize.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBAutoSizeLabel:
public juce::Label,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int const _textWidth;
public:
  FBAutoSizeLabel(std::string const& text);
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};