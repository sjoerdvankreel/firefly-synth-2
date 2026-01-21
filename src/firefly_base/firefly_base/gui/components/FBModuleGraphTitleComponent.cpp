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
  float fps = 10.0f; // no need to run at 60, it distracts
  auto now = std::chrono::high_resolution_clock::now();
  auto elapsedMillis = duration_cast<std::chrono::milliseconds>(now - _updated);
  if (elapsedMillis.count() >= 1000.0 / fps)
  {
    _updated = now;
    _title.clear();
    _subText.clear();
    _title.append(_data->graphs[_graphIndex].title);
    _subText.append(_data->graphs[_graphIndex].subtext);
  }

  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  auto const& scheme = lnf->FindColorSchemeFor(*this);
  g.setColour(scheme.text);
  g.setFont(lnf->GetFont());
  g.drawText(_title, getLocalBounds(), Justification::centredLeft, false);
  g.drawText(_subText, getLocalBounds(), Justification::centredRight, false);
}