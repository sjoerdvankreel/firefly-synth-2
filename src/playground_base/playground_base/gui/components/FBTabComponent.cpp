#include <playground_base/gui/shared/FBGUI.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/components/FBTabComponent.hpp>

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
  return dynamic_cast<IFBHorizontalAutoSize&>(*getTabContentComponent(0)).FixedWidth(height - 30);
}