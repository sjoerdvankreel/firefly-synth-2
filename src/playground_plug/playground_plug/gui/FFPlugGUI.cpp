#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(
  FBRuntimeTopo const* topo, 
  IFBHostGUIContext* hostContext):
FBPlugGUI()
{
  addAndMakeVisible(StoreComponent(FFMakeMasterGUI(topo, this, hostContext)));
}

void 
FFPlugGUI::resized()
{
  getChildComponent(0)->resized();
}