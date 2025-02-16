#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/controls/FBOutputParamLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeAudioParam.hpp>

using namespace juce;

FBOutputParamLabel::
FBOutputParamLabel(
  FBPlugGUI* plugGUI, 
  FBRuntimeAudioParam const* param,
  std::string const& defaultText,
  std::string const& maxWidthText):
Label(),
FBParamControl(plugGUI, param),
_maxTextWidth(FBGetStringWidthCached(maxWidthText))
{
  setText(defaultText, dontSendNotification);
}

void
FBOutputParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String 
FBOutputParamLabel::getTooltip()
{
  return _plugGUI->GetTooltipForParam(_param->runtimeParamIndex);
}

int
FBOutputParamLabel::FixedWidth(int height) const
{
  return _maxTextWidth + getBorderSize().getLeftAndRight();
}

void
FBOutputParamLabel::SetValueNormalizedFromHost(float normalized)
{
  setText(_param->static_.NormalizedToText(FBValueTextDisplay::Text, normalized), dontSendNotification);
}