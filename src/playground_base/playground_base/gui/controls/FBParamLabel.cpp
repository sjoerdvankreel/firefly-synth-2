#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

#include <cmath>
#include <string>

using namespace juce;

FBParamLabel::
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
Label(),
FBParamComponent(plugGUI, param),
IFBHorizontalAutoSize()
{
  setText(param->shortName, dontSendNotification);
  _textWidth = (int)std::ceil(TextLayout::getStringWidth(getFont(), getText()));
}

void 
FBParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

int 
FBParamLabel::FixedWidth(int height) const
{
  return getBorderSize().getLeftAndRight() + _textWidth;
}