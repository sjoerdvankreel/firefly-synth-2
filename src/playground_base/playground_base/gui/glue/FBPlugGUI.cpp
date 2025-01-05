#include <playground_base/gui/glue/FBPlugGUI.hpp>

FBPlugGUI::
FBPlugGUI(IFBHostGUIContext* hostContext) : 
_hostContext(hostContext)
{
  setVisible(true);
}

int 
FBPlugGUI::DefaultHeight() const
{
  int w = DefaultWidth();
  int arW = AspectRatioWidth();
  int arH = AspectRatioHeight();
  return w * arH / arW;
}