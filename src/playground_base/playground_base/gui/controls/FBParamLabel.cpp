#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

#include <cmath>
#include <string>

using namespace juce;

FBParamLabel::
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
Label(),
FBParamComponent(plugGUI, param),
IFBHorizontalAutoSize(),
_textWidth(FBGetStringWidthCached(param->shortName))
{
  setText(param->shortName, dontSendNotification);
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