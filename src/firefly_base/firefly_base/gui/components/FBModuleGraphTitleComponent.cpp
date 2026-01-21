#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/components/FBModuleGraphTitleComponent.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>

using namespace juce;

FBModuleGraphTitleComponent::
FBModuleGraphTitleComponent(FBPlugGUI* plugGUI, FBModuleGraphComponentData const* data, int graphIndex):
_plugGUI(plugGUI), _data(data), _graphIndex(graphIndex) {}

int 
FBModuleGraphTitleComponent::FixedHeight() const
{
  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  return lnf->GetFontHeight() + 4;
}

void 
FBModuleGraphTitleComponent::paint(Graphics& g)
{
  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  auto const& scheme = lnf->FindColorSchemeFor(*this);
  g.setColour(scheme.text);
  g.setFont(lnf->GetFont());
  g.drawText(_data->graphs[_graphIndex].title, getLocalBounds(), Justification::centredLeft, false);
  g.drawText(_data->graphs[_graphIndex].subtext, getLocalBounds(), Justification::centredRight, false);
}