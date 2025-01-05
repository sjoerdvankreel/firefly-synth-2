#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(IFBHostGUIContext* hostContext):
FBPlugGUI(hostContext)
{
  setSize(DefaultWidth(), DefaultHeight());
  _s.setVisible(true);
  addChildComponent(_s);
}

void 
FFPlugGUI::paint(Graphics& g)
{
  g.setColour(Colours::orange);
  g.fillRect(getLocalBounds());
}

void
FFPlugGUI::SetParamNormalized(int index, float normalized)
{
}