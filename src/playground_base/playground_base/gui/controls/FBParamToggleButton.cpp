#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeAudioParam.hpp>

using namespace juce;

FBParamToggleButton::
FBParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeAudioParam const* param):
FBAutoSizeToggleButton(),
FBParamControl(plugGUI, param)
{
  auto* context = plugGUI->HostContext();
  float normalized = context->GetParamNormalized(param->runtimeParamIndex);
  SetValueNormalizedFromHost(normalized);
  _isOn = getToggleState();
}

void 
FBParamToggleButton::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBParamToggleButton::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBParamToggleButton::SetValueNormalizedFromHost(float normalized)
{
  int plain = _param->static_.Boolean().NormalizedToPlain(normalized);
  setToggleState(plain != 0, dontSendNotification);
  _isOn = getToggleState();
}

void
FBParamToggleButton::buttonStateChanged()
{
  float normalized = _param->static_.Boolean().PlainToNormalized(getToggleState());
  int plain = _param->static_.Boolean().NormalizedToPlain(normalized);
  if (_isOn != (plain != 0))
  {
    _plugGUI->HostContext()->PerformImmediateParamEdit(_param->runtimeParamIndex, normalized);
    _plugGUI->SetParamNormalizedFromUI(_param->runtimeParamIndex, normalized);
    _plugGUI->SteppedParamNormalizedChanged(_param->runtimeParamIndex);
  }
  _isOn = getToggleState();
}