#include <playground_base/gui/shared/FBGUILookAndFeel.hpp>

using namespace juce;

void 
FBGUILookAndFeel::drawLinearSliderOutline(
  Graphics& g,
  int x, int y, int width, int height,
  Slider::SliderStyle style, Slider& s)
{
  LookAndFeel_V4::drawLinearSliderOutline(g, x, y, width, height, style, s);
  g.setColour(Colours::red);
  g.fillRect(0, 0, 10, 10);
}