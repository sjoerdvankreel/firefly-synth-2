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
  float normalized = context->GetParamNormalized(param->runtimeParamIndex);
  SetValueNormalizedFromHost(normalized);
  _isOn = getToggleState();
}

void 
FBGUIParamToggleButton::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBGUIParamToggleButton::getTooltip()
{
  return _plugGUI->GetTooltipForParam(_param->runtimeParamIndex);
}

void
FBGUIParamToggleButton::SetValueNormalizedFromHost(float normalized)
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
    _plugGUI->HostContext()->PerformImmediateParamEdit(_param->runtimeParamIndex, normalized);
    _plugGUI->SetParamNormalizedFromUI(_param->runtimeParamIndex, normalized);
    _plugGUI->SteppedParamNormalizedChanged(_param->runtimeParamIndex);
  }
  _isOn = getToggleState();
}