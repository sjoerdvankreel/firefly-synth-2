#pragma once

#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <firefly_base/gui/shared/FBAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FBPlugGUI;

class FBContentComponent:
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
public:
  void resized() override;
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  void SetContent(juce::Component* content);
};

class FBMultiContentComponent :
public juce::Component,
public IFBHorizontalAutoSize,
public IFBVerticalAutoSize
{
  FBContentComponent _component = {};
  std::vector<juce::Component*> _content = {};

public:  
  void resized() override;
  int FixedHeight() const override;
  int FixedWidth(int height) const override;
  
  void SelectContentIndex(int index);
  void SetContent(int index, juce::Component* content);
};