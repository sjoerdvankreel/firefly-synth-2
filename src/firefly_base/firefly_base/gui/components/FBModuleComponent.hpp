#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

// Tags another component with module topo.
// Useful for finding ancestors when applying theme colors.
class FBModuleComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
  int const _moduleIndex;
  int const _moduleSlot;
public:
  int ModuleIndex() const { return _moduleIndex; }
  int ModuleSlot() const { return _moduleSlot; }

  void resized() override;
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  FBModuleComponent(int moduleIndex, int moduleSlot, juce::Component* content);
};