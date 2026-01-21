#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/components/FBModuleGraphTitleComponent.hpp>

using namespace juce;

FBModuleGraphTitleComponent::
FBModuleGraphTitleComponent(FBPlugGUI* plugGUI, FBModuleGraphComponentData const* data, int graphIndex):
_plugGUI(plugGUI), _data(data), _graphIndex(graphIndex) {}

int 
FBModuleGraphTitleComponent::FixedHeight() const
{
  return 20; // TODO
}

void 
FBModuleGraphTitleComponent::paint(Graphics& g)
{
  g.drawText(_data->graphs[_graphIndex].title, getLocalBounds(), Justification::left, false);
}