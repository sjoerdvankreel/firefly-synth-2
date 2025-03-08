#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBAutoSizeSlider.hpp>

using namespace juce;

FBAutoSizeSlider::
FBAutoSizeSlider(FBPlugGUI* plugGUI, Slider::SliderStyle style):
Slider(style, Slider::NoTextBox)
{
  setPopupDisplayEnabled(true, false, plugGUI);
}

int 
FBAutoSizeSlider::FixedWidth(int height) const
{
  if(getSliderStyle() == SliderStyle::RotaryVerticalDrag)
    return height;
  return 0;
}