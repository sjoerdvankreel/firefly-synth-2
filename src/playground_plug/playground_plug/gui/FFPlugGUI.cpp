#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(
  FBRuntimeTopo const* topo, 
  IFBHostGUIContext* hostContext):
FBPlugGUI(),
_topo(topo),
_hostContext(hostContext)
{
  // 29  = master gain
 // auto slider = std::make_unique<juce::Slider>(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox).release();
  
   // addAndMakeVisible(*slider);
  //addAndMakeVisible(_slider);
  setSize(DefaultWidth(), DefaultHeight());
  resized();
}

void 
FFPlugGUI::resized()
{
  //getChildComponent(0)->setBounds(getLocalBounds());
  //getChildComponent(0)->resized();
}

void
FFPlugGUI::SetParamNormalized(int index, float normalized)
{
}