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
  float normalized = context->GetAudioParamNormalized(param->runtimeParamIndex);
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

// TODO double
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
  double normalized = _param->static_.NonRealTime().PlainToNormalized(getToggleState()? 1.0: 0.0);
  bool plain = _param->static_.Boolean().NormalizedToPlain(static_cast<float>(normalized));
  if (_isOn != plain)
  {
    _plugGUI->HostContext()->PerformImmediateAudioParamEdit(_param->runtimeParamIndex, normalized);
    _plugGUI->AudioParamNormalizedChangedFromUI(_param->runtimeParamIndex, normalized);
  }
  _isOn = getToggleState();
}