#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(IFBHostGUIContext* hostContext):
FBPlugGUI(hostContext)
{
  _s.setVisible(true);
  addChildComponent(_s);
  setSize(DefaultWidth(), DefaultHeight());
}

void
FFPlugGUI::SetParamNormalized(int index, float normalized)
{
}