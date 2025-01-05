#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(IFBHostGUIContext* hostContext):
FBPlugGUI(hostContext),
_slider(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox)
{
  addAndMakeVisible(_slider);
  setSize(DefaultWidth(), DefaultHeight());
  resized();
}

void 
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

void
FFPlugGUI::SetParamNormalized(int index, float normalized)
{
}