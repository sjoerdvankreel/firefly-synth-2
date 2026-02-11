#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/components/FBModuleGraphTitleComponent.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>

using namespace juce;

static int const GainSlidingWindowSize = 10;

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
    auto const& gd = _data->graphs[_graphIndex];
    _mainText.clear();
    _mainText.append((gd.anyExchangeActive && gd.exchangeMainText.size()) ? gd.exchangeMainText : gd.defaultMainText);

    float gain = 0.0f;
    bool haveGain = false;
    _titleAndGainText.clear();
    _titleAndGainText.append(gd.title);
    if (gd.anyExchangeActive)
    {
      haveGain = true;
      gain = gd.exchangeGainValue;
    }
    else
    {
      gain = gd.defaultGainValue;
      haveGain = gd.hasDefaultGainValue;
    }
    if (haveGain)
    {
      _gainSlidingWindow.push_back(gain);
      if (_gainSlidingWindow.size() > GainSlidingWindowSize)
        _gainSlidingWindow.erase(_gainSlidingWindow.begin());
      float sum = 0.0f;
      for (int i = 0; i < _gainSlidingWindow.size(); i++)
        sum += _gainSlidingWindow[i];
      float avgGain = sum / _gainSlidingWindow.size();
      _titleAndGainText.append(", ");
      if(gd.displayGainAsDb)
        _titleAndGainText.append(FBGainToStringDb(avgGain, 2));
      else
        _titleAndGainText.append(FBToStringPercent(avgGain, 2));
    }
  }

  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  auto const& scheme = lnf->FindColorSchemeFor(*this);
  g.setColour(scheme.text);
  g.setFont(lnf->GetFont());
  g.drawText(_titleAndGainText, getLocalBounds(), Justification::centredLeft, false);
  g.drawText(_mainText, getLocalBounds(), Justification::centredRight, false);
}