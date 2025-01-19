#include <playground_base/gui/components/FBModuleTabComponent.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <string>

using namespace juce;

FBModuleTabComponent::
FBModuleTabComponent(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext, int moduleIndex, 
  FBModuleTabFactory const& tabFactory) :
  Component(),
_tabs(TabbedButtonBar::Orientation::TabsAtTop)
{
  auto const& module = topo->static_.modules[moduleIndex];
  if(module.slotCount == 1)
    _tabs.addTab(module.name, Colours::black, &tabFactory(topo, plugGUI, hostContext, 0), false);
  else
  {
    _tabs.addTab(module.name, Colours::black, nullptr, false);
    for (int i = 0; i < module.slotCount; i++)
      _tabs.addTab(std::to_string(i + 1), Colours::black, &tabFactory(topo, plugGUI, hostContext, i), false);
    _tabs.setCurrentTabIndex(1);
  }
  addAndMakeVisible(_tabs);
}

void
FBModuleTabComponent::resized()
{
  _tabs.setBounds(getLocalBounds());
  _tabs.resized();
}