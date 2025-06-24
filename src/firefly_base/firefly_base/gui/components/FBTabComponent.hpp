#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FBPlugGUI;

class FBTabBarButton:
public juce::TabBarButton
{
public:
  bool centerText = false;
  FBTabBarButton(const juce::String& name, juce::TabbedButtonBar& bar);
};

class FBTabComponent:
public juce::TabbedComponent
{
public:
  FBTabComponent();

  juce::TabBarButton* 
  createTabButton(const juce::String& tabName, int tabIndex) override;
};

class FBModuleTabComponent:
public FBTabComponent,
public IFBHorizontalAutoSize
{
  FBPlugGUI* const _plugGUI;
  FBRuntimeGUIParam const* const _param;
  std::vector<FBTopoIndices> _moduleIndices = {};

public:
  FBModuleTabComponent(
    FBPlugGUI* plugGUI, 
    FBRuntimeGUIParam const* param);
  
  int FixedWidth(
    int height) const override;

  void AddModuleTab(
    bool centerText, 
    FBTopoIndices const& moduleIndices,    
    juce::Component* component);

  void currentTabChanged(
    int newCurrentTabIndex, 
    juce::String const& newCurrentTabName) override;
};