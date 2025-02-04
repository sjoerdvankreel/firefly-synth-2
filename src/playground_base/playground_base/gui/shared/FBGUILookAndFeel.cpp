#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/shared/FBGUILookAndFeel.hpp>

using namespace juce;

void 
FBGUILookAndFeel::DrawLinearSliderExchangeThumb(
  Graphics& g, FBParamSlider& slider, 
  int y, int height, float exchangeValue) 
{
  auto layout = getSliderLayout(slider);
  int sliderRegionStart = layout.sliderBounds.getX();
  int sliderRegionSize = layout.sliderBounds.getWidth();
  float minExchangePos = (float)(sliderRegionStart + exchangeValue * sliderRegionSize);

  float kx = minExchangePos;
  float ky = (float)y + (float)height * 0.5f;
  Point<float> maxPoint = { kx, ky };
  float thumbWidth = (float)getSliderThumbRadius(slider);
  g.setColour(slider.findColour(Slider::thumbColourId).withAlpha(0.5f));
  g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(maxPoint));
}

void 
FBGUILookAndFeel::drawLinearSlider(
  Graphics& g,
  int x, int y, int width, int height,
  float sliderPos, float minSliderPos, float maxSliderPos,
  Slider::SliderStyle style, Slider& s)
{
  FBParamSlider* paramSlider;
  LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, s);
  if ((paramSlider = dynamic_cast<FBParamSlider*>(&s)) == nullptr)
    return;    
  DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramSlider->MinExchangeValue());
  DrawLinearSliderExchangeThumb(g, *paramSlider, y, height, paramSlider->MaxExchangeValue());
}