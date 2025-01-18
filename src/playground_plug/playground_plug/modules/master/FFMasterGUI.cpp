#include <playground_plug/gui/FFPlugGUIStore.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>

using namespace juce;

std::unique_ptr<Component>
FFMakeMasterGUI(FBRuntimeTopo const* topo, IFFPlugGUIStore* store, IFBHostGUIContext* hostContext)
{
  Label& lbl = FFGUIStoreComponent<Label>(store);
  return {};
}