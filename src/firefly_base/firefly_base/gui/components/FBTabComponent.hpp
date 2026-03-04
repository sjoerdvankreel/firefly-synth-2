#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBContentComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/base/topo/runtime/FBTopoIndices.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <map>

class FBPlugGUI;
struct FBTheme;
struct FBColorScheme;  
struct FBRuntimeGUIParam;

inline int constexpr FBTabBarDepth = 20;
inline int constexpr FBTabBarDepthBig = 26;

class FBTabBarButton:
public juce::TabBarButton
{
public:
  bool large = false;
  bool centerText = false;

  FBTabBarButton(
    const juce::String& name, 
    juce::TabbedButtonBar& bar);

  virtual bool IsModuleTab() const { return false; }
  void clicked(const juce::ModifierKeys& modifiers) override;
};

class FBModuleTabBarButton:
public FBTabBarButton,
public IFBThemingComponent
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

  bool IsModuleTab() const override { return true; }
  void clicked(const juce::ModifierKeys& modifiers) override;
  FBColorScheme const* GetScheme(FBTheme const& theme) const override;
  std::string const GetSeparatorText() const { return _separatorText; }
};

class FBAutoSizeTabComponent:
public juce::TabbedComponent,
public IFBHorizontalAutoSize
{
protected:
  FBPlugGUI* const _plugGUI;
  bool const _big;

public:
  FBAutoSizeTabComponent(FBPlugGUI* plugGUI);
  FBAutoSizeTabComponent(FBPlugGUI* plugGUI, bool big);

  bool Big() const { return _big; }
  int FixedWidth(int height) const override;
  void AddTab(std::string const& header, bool centerText, juce::Component* component);
  
  juce::TabBarButton* createTabButton(const juce::String& tabName, int tabIndex) override;
  void currentTabChanged(int newCurrentTabIndex, const juce::String& newCurrentTabName) override;
};

class FBModuleTabComponent:
public FBAutoSizeTabComponent
{
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

class FBSelectComponent:
public juce::Component
{
  FBPlugGUI* const _plugGUI;
  std::unique_ptr<FBGridComponent> _mainGrid = {};
  std::unique_ptr<FBGridComponent> _selectGrid = {};
  std::unique_ptr<FBContentComponent> _content = {};
  std::vector<std::unique_ptr<FBAutoSizeLabel>> _labels = {};
  std::vector<std::unique_ptr<FBAutoSizeButton>> _buttons = {};

  void Select(FBAutoSizeButton* button, juce::Component* component);

public:
  FBSelectComponent(
    FBPlugGUI* plugGUI, 
    std::vector<int> const& rows, std::vector<int> const& cols);

  void resized() override;
  void AddLabel(int row, int col, std::string const& text);
  void AddSelector(int row, int col, std::string const& text, juce::Component* component);
};