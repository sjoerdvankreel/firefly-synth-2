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
    _mainText.clear();
    _titleAndSubText.clear();

    auto const& gd = _data->graphs[_graphIndex];
    _titleAndSubText.append(gd.title);
    auto const& subText = (gd.anyExchangeActive && gd.exchangeSubText.size()) ? gd.exchangeSubText : gd.defaultSubText;
    if (subText.size())
    {
      _titleAndSubText.append(", ");
      _titleAndSubText.append(subText);
    }
    _mainText.append((gd.anyExchangeActive && gd.exchangeMainText.size()) ? gd.exchangeMainText : gd.defaultMainText);
  }

  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  auto const& scheme = lnf->FindColorSchemeFor(*this);
  g.setColour(scheme.text);
  g.setFont(lnf->GetFont());
  g.drawText(_titleAndSubText, getLocalBounds(), Justification::centredLeft, false);
  g.drawText(_mainText, getLocalBounds(), Justification::centredRight, false);
}