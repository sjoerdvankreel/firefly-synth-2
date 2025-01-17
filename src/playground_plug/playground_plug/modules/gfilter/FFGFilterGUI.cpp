#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>

using namespace juce;

std::unique_ptr<Component>
FFMakeGFilterGUI(FBRuntimeTopo const* topo, IFBHostGUIContext* hostContext)
{
  auto result = std::make_unique<TabbedComponent>(TabbedButtonBar::Orientation::TabsAtTop);
  return result;
}