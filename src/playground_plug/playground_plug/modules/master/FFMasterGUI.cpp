#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>

using namespace juce;

std::unique_ptr<Component>
FFMakeMasterGUI(FBRuntimeTopo const* topo, IFBGUIStore* store, IFBHostGUIContext* hostContext)
{
  Label& lbl = FBGUIStoreComponent<Label>(store);
  return {};
}