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
  bool const _isOverlay;
  int _moduleSlot = -1;
  int _moduleIndex = -1;
  juce::Component* _content = nullptr;

public:
  int ModuleSlot() const { return _moduleSlot; }
  int ModuleIndex() const { return _moduleIndex; }

  void resized() override;
  void paint(juce::Graphics& g) override;

  int FixedHeight() const override;
  int FixedWidth(int height) const override;

  FBModuleComponent(bool isOverlay);
  FBModuleComponent(int moduleIndex, int moduleSlot, juce::Component* content);
  void SetModuleContent(int moduleIndex, int moduleSlot, juce::Component* content);
};