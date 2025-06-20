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

FBTabComponent::
FBTabComponent():
TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop)
{
  setTabBarDepth(20);
  setLookAndFeel(FBGetLookAndFeel());
}

TabBarButton*
FBTabComponent::createTabButton(const juce::String& tabName, int /*tabIndex*/)
{
  return new FBTabBarButton(tabName, *tabs);
}

FBModuleTabComponent::
FBModuleTabComponent(FBPlugGUI* plugGUI):
FBTabComponent(),
_plugGUI(plugGUI) {}

int
FBModuleTabComponent::FixedWidth(int height) const
{
  auto& content = dynamic_cast<IFBHorizontalAutoSize&>(*getTabContentComponent(0));
  return content.FixedWidth(height - 20);
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

void
FBModuleTabComponent::currentTabChanged(
  int newCurrentTabIndex, juce::String const& /*newCurrentTabName*/)
{
  if (newCurrentTabIndex < 0 || newCurrentTabIndex >= _moduleIndices.size())
    return;
  auto const& indices = _moduleIndices[newCurrentTabIndex];
  _plugGUI->ActiveModuleSlotChanged(indices.index, indices.slot);
}