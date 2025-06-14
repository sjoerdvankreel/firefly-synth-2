#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBOutputLabel.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBOutputParamLabel::
FBOutputParamLabel(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param, 
  std::string const& defaultText,
  std::string const& maxWidthText):
Label(),
FBParamControl(plugGUI, param),
_maxTextWidth(FBGUIGetStringWidthCached(maxWidthText))
{
  setText(defaultText, dontSendNotification);
}

void
FBOutputParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

int
FBOutputParamLabel::FixedWidth(int /*height*/) const
{
  return _maxTextWidth + getBorderSize().getLeftAndRight();
}

String
FBOutputParamLabel::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBOutputParamLabel::SetValueNormalizedFromHost(double normalized)
{
  setText(_param->static_.NormalizedToTextWithUnit(false, normalized), dontSendNotification);
}