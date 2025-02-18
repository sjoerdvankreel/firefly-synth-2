#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBGUIParamToggleButton.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeGUIParam.hpp>

using namespace juce;

FBGUIParamToggleButton::
FBGUIParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBAutoSizeToggleButton(),
FBGUIParamControl(plugGUI, param)
{
  auto* context = plugGUI->HostContext();
  float normalized = context->GetGUIParamNormalized(param->runtimeParamIndex); 
  SetValueNormalizedFromPlug(normalized);
  _isOn = getToggleState();
}

String
FBGUIParamToggleButton::getTooltip()
{
  return _plugGUI->GetTooltipForGUIParam(_param->runtimeParamIndex);
}

void
FBGUIParamToggleButton::SetValueNormalizedFromPlug(float normalized)
{
  int plain = _param->static_.Boolean().NormalizedToPlain(normalized);
  setToggleState(plain != 0, dontSendNotification);
  _isOn = getToggleState();
}

void
FBGUIParamToggleButton::buttonStateChanged()
{
  float normalized = _param->static_.Boolean().PlainToNormalized(getToggleState());
  int plain = _param->static_.Boolean().NormalizedToPlain(normalized);
  if (_isOn != (plain != 0))
  {
    _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, normalized);
    _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, normalized);
  }
  _isOn = getToggleState();
}