#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBOutputParamLabel.hpp>

using namespace juce;

FBOutputParamLabel::
FBOutputParamLabel(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param, 
  std::string const& defaultText,
  std::string const& maxWidthText):
Label(),
FBParamControl(plugGUI, param),
_maxTextWidth((int)std::ceil(TextLayout::getStringWidth(Font(12.0f), maxWidthText)))
{
  setText(defaultText, dontSendNotification);
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
  setText(_param->static_.NormalizedToText(FBTextDisplay::Text, normalized), dontSendNotification);
}