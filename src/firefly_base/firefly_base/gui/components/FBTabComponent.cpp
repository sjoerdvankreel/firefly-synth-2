#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/base/topo/runtime/FBTopoDetail.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

#include <map>
#include <string>

using namespace juce;

class FBModuleTabbedButtonBar:
public TabbedButtonBar
{
  FBModuleTabComponent* const _owner;

public:
  FBModuleTabbedButtonBar(FBModuleTabComponent* owner, TabbedButtonBar::Orientation o);

  Colour getTabBackgroundColour(int tabIndex);
  TabBarButton* createTabButton(const String& tabName, int tabIndex);
  void popupMenuClickOnTab(int tabIndex, const String& tabName) override;
  void currentTabChanged(int newCurrentTabIndex, const String& newTabName) override;
};

FBModuleTabbedButtonBar::
FBModuleTabbedButtonBar(FBModuleTabComponent* owner, TabbedButtonBar::Orientation o) :
TabbedButtonBar(o), _owner(owner) {}

Colour
FBModuleTabbedButtonBar::getTabBackgroundColour(int tabIndex)
{
  return _owner->getTabs()->getTabBackgroundColour(tabIndex);
}

TabBarButton*
FBModuleTabbedButtonBar::createTabButton(const String& tabName, int tabIndex)
{
  return _owner->createTabButton(tabName, tabIndex);
}

void
FBModuleTabbedButtonBar::popupMenuClickOnTab(int tabIndex, const String& tabName)
{
  _owner->popupMenuClickOnTab(tabIndex, tabName);
}

void 
FBModuleTabbedButtonBar::currentTabChanged(int newCurrentTabIndex, const String& newTabName)
{
  _owner->changeCallback(newCurrentTabIndex, newTabName);
}

FBTabBarButton::
FBTabBarButton(const String& name, TabbedButtonBar& bar):
TabBarButton(name, bar) {}

void 
FBTabBarButton::clicked(const ModifierKeys& modifiers)
{
  FBModuleTabComponent* tabs;
  if (!modifiers.isRightButtonDown())
    TabBarButton::clicked(modifiers);
  else if ((tabs = findParentComponentOfClass<FBModuleTabComponent>()) != nullptr)
    tabs->TabRightClicked(getIndex());
}

FBModuleTabBarButton::
FBModuleTabBarButton(
FBPlugGUI* plugGUI,
const juce::String& name,
juce::TabbedButtonBar& bar,
FBTopoIndices const& moduleIndices):
FBTabBarButton(name, bar),
_plugGUI(plugGUI),
_moduleIndices(moduleIndices) {}

void
FBModuleTabBarButton::clicked(const ModifierKeys& modifiers)
{
  FBTabBarButton::clicked(modifiers);
  _plugGUI->ModuleSlotClicked(_moduleIndices.index, _moduleIndices.slot);
}

FBAutoSizeTabComponent::
FBAutoSizeTabComponent():
TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop)
{
  setTabBarDepth(FBTabBarDepth);
  setLookAndFeel(FBGetLookAndFeel());
}

int
FBAutoSizeTabComponent::FixedWidth(int height) const
{
  auto& content = dynamic_cast<IFBHorizontalAutoSize&>(*getTabContentComponent(0));
  return content.FixedWidth(height - 20);
}

TabBarButton*
FBAutoSizeTabComponent::createTabButton(const juce::String& tabName, int /*tabIndex*/)
{
  return new FBTabBarButton(tabName, *tabs);
}

FBModuleTabComponent::
FBModuleTabComponent(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBAutoSizeTabComponent(),
_plugGUI(plugGUI),
_param(param)
{
  removeChildComponent(tabs.get());
  tabs.reset(new FBModuleTabbedButtonBar(this, TabbedButtonBar::Orientation::TabsAtTop));
  addAndMakeVisible(tabs.get());
  assert(param != nullptr);
  double normalized = _plugGUI->HostContext()->GetGUIParamNormalized(_param->runtimeParamIndex);
  _storedSelectedTab = _param->static_.Discrete().NormalizedToPlainFast((float)normalized);
}

TabBarButton*
FBModuleTabComponent::createTabButton(const juce::String& tabName, int tabIndex)
{
  return new FBModuleTabBarButton(_plugGUI, tabName, *tabs, _moduleIndices[tabIndex]);
}

void
FBModuleTabComponent::ActivateStoredSelectedTab()
{
  if (!(0 <= _storedSelectedTab && _storedSelectedTab < _moduleIndices.size()))
    return;
  setCurrentTabIndex(_storedSelectedTab);
}

void
FBModuleTabComponent::currentTabChanged(
  int newCurrentTabIndex, juce::String const& /*newCurrentTabName*/)
{
  if (newCurrentTabIndex < 0 || newCurrentTabIndex >= _moduleIndices.size())
    return;
  auto const& indices = _moduleIndices[newCurrentTabIndex];
  _plugGUI->ActiveModuleSlotChanged(indices.index, indices.slot);
  double normalized = _param->static_.Discrete().PlainToNormalizedFast(newCurrentTabIndex);
  _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, normalized);
  _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, normalized);
}

void
FBModuleTabComponent::AddModuleTab(
  bool centerText, 
  FBTopoIndices const& moduleIndices,
  Component* component)
{
  _moduleIndices.push_back(moduleIndices);
  auto topo = _plugGUI->HostContext()->Topo();
  auto const& module = topo->static_->modules[moduleIndices.index];

  std::string header = {};
  if (module.tabSlotFormatter != nullptr)
  {
    header = FBMakeRuntimeModuleShortName(
      *topo->static_, module.name, module.slotCount,
      moduleIndices.slot, module.tabSlotFormatter, module.slotFormatterOverrides);
  }
  else
  {
    header = std::to_string(moduleIndices.slot + 1);
    if (moduleIndices.slot == 0)
      if (module.slotCount > 1)
        header = module.name + " " + header;
      else
        header = module.name;
  }

  addTab(header, Colours::black, component, false);
  auto button = getTabbedButtonBar().getTabButton(static_cast<int>(_moduleIndices.size() - 1));
  dynamic_cast<FBTabBarButton&>(*button).centerText = centerText;
}

void
FBModuleTabComponent::changeCallback(
  int newCurrentTabIndex,
  juce::String const& newTabName)
{
  auto* newPanelComp = getTabContentComponent(getCurrentTabIndex());

  if (newPanelComp != _panelComponent)
  {
    if (_panelComponent != nullptr)
    {
      _panelComponent->setVisible(false);
      removeChildComponent(_panelComponent);
    }

    _panelComponent = newPanelComp;

    if (_panelComponent != nullptr)
    {
      // do these ops as two stages instead of addAndMakeVisible() so that the
      // component has always got a parent when it gets the visibilityChanged() callback
      addChildComponent(_panelComponent);
      _panelComponent->sendLookAndFeelChange();
      _panelComponent->setVisible(true);
      _panelComponent->toFront(true);
    }

    repaint();
  }

  resized();
  currentTabChanged(newCurrentTabIndex, newTabName);
}

void 
FBModuleTabComponent::TabRightClicked(int tabIndex)
{
  if (tabIndex < 0 || tabIndex >= _moduleIndices.size())
    return;
  
  auto moduleIndices = _moduleIndices[tabIndex];
  auto const& staticTopo = *_plugGUI->HostContext()->Topo()->static_;
  auto const& staticModule = staticTopo.modules[moduleIndices.index];

  PopupMenu menu;
  menu.addItem(1, "Clear");
  if (staticModule.slotCount > 1)
  {
    PopupMenu subMenu;
    for (int i = 0; i < staticModule.slotCount; i++)
    {
      auto name = FBMakeRuntimeModuleShortName(
        staticTopo, staticModule.name, staticModule.slotCount, i, 
        staticModule.slotFormatter, staticModule.slotFormatterOverrides);
      subMenu.addItem(2 + i, name, moduleIndices.slot != i);
    }
    menu.addSubMenu("Copy To", subMenu);
  }

  PopupMenu::Options options;
  options = options.withParentComponent(_plugGUI);
  options = options.withTargetComponent(getTabbedButtonBar().getTabButton(tabIndex));
  menu.showMenuAsync(options, [this, moduleIndices, slotCount = staticModule.slotCount](int id) {
    if (id == 1) 
      _plugGUI->HostContext()->ClearModuleAudioParams(moduleIndices); 
    else if(2 <= id && id < slotCount + 2) 
      _plugGUI->HostContext()->CopyModuleAudioParams(moduleIndices, id - 2);
  });
}