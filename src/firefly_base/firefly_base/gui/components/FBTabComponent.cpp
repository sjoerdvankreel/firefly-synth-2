#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>

#include <string>

using namespace juce;

FBTabBarButton::
FBTabBarButton(const String& name, TabbedButtonBar& bar):
TabBarButton(name, bar) {}

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
  assert(param != nullptr);
  double normalized = _plugGUI->HostContext()->GetGUIParamNormalized(_param->runtimeParamIndex);
  _storedSelectedTab = _param->static_.Discrete().NormalizedToPlainFast((float)normalized);
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
  auto const& module = topo->static_.modules[moduleIndices.index];
  std::string header = std::to_string(moduleIndices.slot + 1);
  if (moduleIndices.slot == 0)
    if (module.slotCount > 1)
      header = module.tabName + " " + header;
    else
      header = module.tabName;
  addTab(header, Colours::black, component, false);
  auto button = getTabbedButtonBar().getTabButton(static_cast<int>(_moduleIndices.size() - 1));
  dynamic_cast<FBTabBarButton&>(*button).centerText = centerText;
}