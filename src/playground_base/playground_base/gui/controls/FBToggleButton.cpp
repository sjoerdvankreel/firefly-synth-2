#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBToggleButton.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

int 
FBAutoSizeToggleButton::FixedHeight() const
{
  return 24;
}

int 
FBAutoSizeToggleButton::FixedWidth(int height) const
{
  return 24;
}

FBGUIParamToggleButton::
FBGUIParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBAutoSizeToggleButton(),
FBGUIParamControl(plugGUI, param)
{
  auto* context = plugGUI->HostContext();
  double normalized = context->GetGUIParamNormalized(param->runtimeParamIndex);
  SetValueNormalizedFromPlug(normalized);
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
  return _plugGUI->GetTooltipForGUIParam(_param->runtimeParamIndex);
}

void
FBGUIParamToggleButton::SetValueNormalizedFromPlug(double normalized)
{
  bool plain = _param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
  setToggleState(plain, dontSendNotification);
  _isOn = getToggleState();
}

void
FBGUIParamToggleButton::buttonStateChanged()
{
  double normalized = _param->static_.NonRealTime().PlainToNormalized(getToggleState()? 1.0: 0.0);
  bool plain = _param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
  if (_isOn != plain)
  {
    _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, normalized);
    _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, normalized);
  }
  _isOn = getToggleState();
}

FBParamToggleButton::
FBParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBAutoSizeToggleButton(),
FBParamControl(plugGUI, param)
{
  auto* context = plugGUI->HostContext();
  SetValueNormalizedFromHost(context->GetAudioParamNormalized(param->runtimeParamIndex));
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
  bool plain = _param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
  setToggleState(plain, dontSendNotification);
  _isOn = getToggleState();
}

void
FBParamToggleButton::buttonStateChanged()
{
  double normalized = _param->static_.NonRealTime().PlainToNormalized(getToggleState()? 1.0: 0.0);
  bool plain = _param->static_.Boolean().NormalizedToPlainFast(static_cast<float>(normalized));
  if (_isOn != plain)
  {
    _plugGUI->HostContext()->PerformImmediateAudioParamEdit(_param->runtimeParamIndex, normalized);
    _plugGUI->AudioParamNormalizedChangedFromUI(_param->runtimeParamIndex, normalized);
  }
  _isOn = getToggleState();
}