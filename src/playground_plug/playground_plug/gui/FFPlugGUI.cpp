#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

FFPlugGUI::
FFPlugGUI(IFBHostGUIContext* hostContext):
FBPlugGUI(hostContext)
{
  setSize(DefaultWidth(), DefaultHeight());
  _s.setVisible(true);
  addChildComponent(_s);
}

void 
FFPlugGUI::SetParamNormalized(int index, float normalized)
{
}