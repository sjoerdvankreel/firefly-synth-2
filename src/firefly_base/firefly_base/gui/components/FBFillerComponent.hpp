#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

// Lazy hack for when the autosizing is not just right.
class FBFillerComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
  int const _w;
  int const _h;
public:
  FBFillerComponent(int w, int h);
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
};