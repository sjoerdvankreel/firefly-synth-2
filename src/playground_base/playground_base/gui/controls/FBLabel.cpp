#include <playground_base/gui/shared/FBGUI.hpp>
#include <playground_base/gui/controls/FBLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBAutoSizeLabel::
FBAutoSizeLabel(std::string const& text):
Label(),
IFBHorizontalAutoSize(),
_textWidth(FBGUIGetStringWidthCached(text))
{
  setText(text, dontSendNotification);
  setBorderSize({ 1, 2, 1, 2 });
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
FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeParam const* param, std::string overrideText):
FBAutoSizeLabel(overrideText.empty()? param->displayName: overrideText),
FBParamComponent(plugGUI, param) {}

void 
FBParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}