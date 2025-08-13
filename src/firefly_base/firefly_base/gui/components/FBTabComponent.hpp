#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamComponent.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <map>

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
  std::string const _separatorText;
  FBTopoIndices const _moduleIndices;

public:
  FBModuleTabBarButton(
    FBPlugGUI* plugGUI,
    std::string const& separatorText,
    const juce::String& name,
    juce::TabbedButtonBar& bar,
    FBTopoIndices const& moduleIndices);

  void clicked(const juce::ModifierKeys& modifiers) override;
  std::string const GetSeparatorText() const { return _separatorText; }
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
  FBRuntimeGUIParam const* const _param;
  int _storedSelectedTab = -1;
  std::vector<FBTopoIndices> _moduleIndices = {};
  std::map<int, std::string> _tabSeparatorText = {};

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
  void SetTabSeparatorText(int tabIndex, std::string const& text);

  void currentTabChanged(
    int newCurrentTabIndex, 
    juce::String const& newCurrentTabName) override;

  juce::TabBarButton*
  createTabButton(const juce::String& tabName, int tabIndex) override;
};