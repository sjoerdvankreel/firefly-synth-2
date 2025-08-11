#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FBPlugGUI;

inline int constexpr FBTabBarDepth = 20;

class FBTabBarButton:
public juce::TabBarButton
{
public:
  bool centerText = false;
  FBTabBarButton(
    const juce::String& name, 
    juce::TabbedButtonBar& bar);

  void clicked(const juce::ModifierKeys& modifiers) override;
};

class FBModuleTabBarButton:
public FBTabBarButton
{
  FBPlugGUI* const _plugGUI;
  FBTopoIndices const _moduleIndices;

public:
  FBModuleTabBarButton(
    FBPlugGUI* plugGUI,
    const juce::String& name,
    juce::TabbedButtonBar& bar,
    FBTopoIndices const& moduleIndices);

  void clicked(const juce::ModifierKeys& modifiers) override;
};

class FBAutoSizeTabComponent:
public juce::TabbedComponent,
public IFBHorizontalAutoSize
{
public:
  FBAutoSizeTabComponent();

  int FixedWidth(int height) const override;

  juce::TabBarButton* 
  createTabButton(const juce::String& tabName, int tabIndex) override;
};

class FBModuleTabComponent:
public FBAutoSizeTabComponent
{
  FBPlugGUI* const _plugGUI;
  int _storedSelectedTab = -1;
  FBRuntimeGUIParam const* const _param;
  std::vector<FBTopoIndices> _moduleIndices = {};

public:
  FBModuleTabComponent(
    FBPlugGUI* plugGUI, 
    FBRuntimeGUIParam const* param);
  
  void AddModuleTab(
    bool centerText,
    FBTopoIndices const& moduleIndices,
    juce::Component* component);
  
  void ActivateStoredSelectedTab();
  void TabRightClicked(int tabIndex);

  void currentTabChanged(
    int newCurrentTabIndex, 
    juce::String const& newCurrentTabName) override;

  juce::TabBarButton*
  createTabButton(const juce::String& tabName, int tabIndex) override;
};