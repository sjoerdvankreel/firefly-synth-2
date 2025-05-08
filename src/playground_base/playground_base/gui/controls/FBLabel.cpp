#include <playground_base/gui/controls/FBLabel.hpp>
#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBAutoSizeLabel::
FBAutoSizeLabel(std::string const& text):
Label(),
IFBHorizontalAutoSize(),
_textWidth(FBGUIGetStringWidthCached(text))
{
  setText(text, dontSendNotification);
}

int 
FBAutoSizeLabel::FixedHeight() const
{
  return 20;
}

int 
FBAutoSizeLabel::FixedWidth(int height) const
{
  return getBorderSize().getLeftAndRight() + _textWidth;
}

FBGUIParamLabel::
FBGUIParamLabel(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param) :
FBAutoSizeLabel(param->displayName),
FBGUIParamComponent(plugGUI, param) {}

void
FBGUIParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

FBParamLabel::
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBAutoSizeLabel(param->displayName),
FBParamComponent(plugGUI, param) {}

void 
FBParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}