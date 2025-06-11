#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>

#include <string>

using namespace juce;

FBTabComponent::
FBTabComponent():
TabbedComponent(TabbedButtonBar::Orientation::TabsAtTop)
{
  setTabBarDepth(20);
  setLookAndFeel(FBGetLookAndFeel());
}

FBModuleTabComponent::
FBModuleTabComponent(
  FBPlugGUI* plugGUI, int moduleIndex, 
  FBModuleTabFactory const& tabFactory) :
FBTabComponent(),
_plugGUI(plugGUI),
_moduleIndex(moduleIndex)
{ 
  auto topo = plugGUI->HostContext()->Topo();
  auto const& module = topo->static_.modules[moduleIndex];
  if(module.slotCount == 1)
    addTab(module.name, Colours::black, tabFactory(plugGUI, 0), false);
  else
  {
    addTab(module.name, Colours::black, nullptr, false);
    for (int i = 0; i < module.slotCount; i++)
      addTab(std::to_string(i + 1), Colours::black, tabFactory(plugGUI, i), false);
    setCurrentTabIndex(1);
  }
}

void 
FBModuleTabComponent::currentTabChanged(
  int newCurrentTabIndex, juce::String const& newCurrentTabName)
{
  // TODO
  if(newCurrentTabIndex > 0)
    _plugGUI->ActiveModuleSlotChanged(_moduleIndex, newCurrentTabIndex - 1);
}

int 
FBModuleTabComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize&>(*getTabContentComponent(0)).FixedWidth(height - 20);
}

FBModuleTabComponent2::
FBModuleTabComponent2(FBPlugGUI* plugGUI):
FBTabComponent(),
_plugGUI(plugGUI) {}

int
FBModuleTabComponent2::FixedWidth(int height) const
{
  auto& content = dynamic_cast<IFBHorizontalAutoSize&>(*getTabContentComponent(0));
  return content.FixedWidth(height - 20);
}

void
FBModuleTabComponent2::AddModuleTab(
  FBTopoIndices const& moduleIndices, Component* component)
{
  _moduleIndices.push_back(moduleIndices);
  auto topo = _plugGUI->HostContext()->Topo();
  auto const& module = topo->static_.modules[moduleIndices.index];
  std::string header = std::to_string(moduleIndices.slot + 1);
  if (moduleIndices.slot == 0)
    header = module.name + " " + header;
  addTab(header, Colours::black, component, false);
}

void
FBModuleTabComponent2::currentTabChanged(
  int newCurrentTabIndex, juce::String const& newCurrentTabName)
{
  if (newCurrentTabIndex < 0 || newCurrentTabIndex >= _moduleIndices.size())
    return;
  auto const& indices = _moduleIndices[newCurrentTabIndex];
  _plugGUI->ActiveModuleSlotChanged(indices.index, indices.slot);
}