#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(
  FBRuntimeTopo const* topo, 
  IFBHostGUIContext* hostContext):
FBPlugGUI()
{
  addAndMakeVisible(FFMakeMasterGUI(topo, 0, this, hostContext));
  resized();
}

void 
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}