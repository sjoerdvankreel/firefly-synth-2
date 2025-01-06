#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
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
  _slider = std::make_unique<FBParamSlider>(&_topo->params[4], _hostContext, Slider::SliderStyle::Rotary);
  addAndMakeVisible(*_slider);
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
  if(index == 4)
    dynamic_cast<FBParamSlider*>(getChildComponent(0))->setValue(normalized, juce::dontSendNotification);
}