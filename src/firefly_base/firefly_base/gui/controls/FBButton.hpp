#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <string>

class FBPlugGUI;

class FBAutoSizeButton final:
public juce::TextButton,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int const _textWidth;
public:
  FBAutoSizeButton(std::string const& text);
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};