#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBParamToggleButton::
FBParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBAutoSizeToggleButton(),
FBParamControl(plugGUI, param)
{
  auto* context = plugGUI->HostContext();
  double normalized = context->GetAudioParamNormalized(param->runtimeParamIndex);
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
FBParamToggleButton::SetValueNormalizedFromHost(double normalized)
{
  // TODO hunt for "float normalized"
  int plain = _param->static_.NonRealTime().NormalizedToPlain(normalized);
  setToggleState(plain != 0, dontSendNotification);
  _isOn = getToggleState();
}

void
FBParamToggleButton::buttonStateChanged()
{
  double normalized = _param->static_.NonRealTime().PlainToNormalized(getToggleState());
  int plain = _param->static_.BooleanRealTime().NormalizedToPlain(normalized);
  if (_isOn != (plain != 0))
  {
    _plugGUI->HostContext()->PerformImmediateAudioParamEdit(_param->runtimeParamIndex, normalized);
    _plugGUI->AudioParamNormalizedChangedFromUI(_param->runtimeParamIndex, normalized);
  }
  _isOn = getToggleState();
}