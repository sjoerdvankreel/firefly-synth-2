#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

// Tags another component with an id to track it in the theming.
class FBThemedComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
  int _componentId;
  juce::Component* _content;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;

  int FixedHeight() const override;
  int FixedWidth(int height) const override;

  int ComponentId() const { return _componentId; }
  FBThemedComponent(int componentId, juce::Component* content);
};

// Tags another component with module topo.
// Useful for finding ancestors when applying theme colors.
class FBModuleComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
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

  FBModuleComponent();
  FBModuleComponent(int moduleIndex, int moduleSlot, juce::Component* content);
  void SetModuleContent(int moduleIndex, int moduleSlot, juce::Component* content);
};