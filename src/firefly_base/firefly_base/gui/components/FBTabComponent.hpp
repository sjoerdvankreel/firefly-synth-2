#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <map>

class FBPlugGUI;
struct FBRuntimeGUIParam;

inline int constexpr FBTabBarDepth = 20;
inline int constexpr FBTabBarDepthBig = 30;

class FBTabBarButton:
public juce::TabBarButton
{
public:
  bool large = false;
  bool centerText = false;
  bool isModuleTab = false;
  FBTopoIndices moduleIndices = {};

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
  bool const _big;

public:
  FBAutoSizeTabComponent();
  FBAutoSizeTabComponent(bool big);

  bool Big() const { return _big; }
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
  juce::PopupMenu extendedMenu = {};
  std::function<void(FBPlugGUI* plugGUI, FBTopoIndices const&, int)> extendedMenuHandler = {};

  FBModuleTabComponent(
    FBPlugGUI* plugGUI, 
    FBRuntimeGUIParam const* param);
  
  void AddModuleTab(
    bool centerText, bool large,
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