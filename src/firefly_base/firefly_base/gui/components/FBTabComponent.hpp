#pragma once

#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>
#include <firefly_base/gui/shared/FBAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>
#include <functional>

class FBPlugGUI;

typedef std::function<juce::Component*(
  FBPlugGUI* plugGUI, int moduleSlot)>
FBModuleTabFactory;

class FBTabComponent:
public juce::TabbedComponent
{
public:
  FBTabComponent();
};

class FBModuleTabComponent:
public FBTabComponent,
public IFBHorizontalAutoSize
{
  FBPlugGUI* const _plugGUI;
  int const _moduleIndex;

public:
  FBModuleTabComponent(
    FBPlugGUI* plugGUI, int moduleIndex, 
    FBModuleTabFactory const& tabFactory);
  
  int FixedWidth(int height) const override;

  void currentTabChanged(
    int newCurrentTabIndex, 
    juce::String const& newCurrentTabName) override;
};

class FBModuleTabComponent2:
public FBTabComponent,
public IFBHorizontalAutoSize
{
  FBPlugGUI* const _plugGUI;
  std::vector<FBTopoIndices> _moduleIndices = {};

public:
  FBModuleTabComponent2(FBPlugGUI* plugGUI);
  
  int FixedWidth(
    int height) const override;

  void AddModuleTab(
    FBTopoIndices const& moduleIndices,
    juce::Component* component);

  void currentTabChanged(
    int newCurrentTabIndex, 
    juce::String const& newCurrentTabName) override;
};