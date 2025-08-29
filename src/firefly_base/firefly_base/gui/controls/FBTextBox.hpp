#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBAutoSizeTextBox:
public juce::TextEditor,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  int const _fixedWidth;

public:
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBAutoSizeTextBox(int fixedWidth);
};
