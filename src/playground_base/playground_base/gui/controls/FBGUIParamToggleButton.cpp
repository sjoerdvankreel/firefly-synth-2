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
  // TODO SetValueNormalizedFromHost(normalized);
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
  // TODO 
  return _plugGUI->GetTooltipForParam(_param->runtimeParamIndex);
}

/* TODO
void
FBGUIParamToggleButton::SetValueNormalizedFromHost(float normalized)
{
  // TODO int plain = _param->static_.Boolean().NormalizedToPlain(normalized);
  // TODO setToggleState(plain != 0, dontSendNotification);
  // TODO _isOn = getToggleState();
}
*/

void
FBGUIParamToggleButton::buttonStateChanged()
{
  // TODO float normalized = _param->static_.Boolean().PlainToNormalized(getToggleState());
  // TODO int plain = _param->static_.Boolean().NormalizedToPlain(normalized);
  // TODO if (_isOn != (plain != 0))
// TODO   {
// TODO     _plugGUI->HostContext()->PerformImmediateParamEdit(_param->runtimeParamIndex, normalized);
// TODO     _plugGUI->SetParamNormalizedFromUI(_param->runtimeParamIndex, normalized);
// TODO     _plugGUI->SteppedParamNormalizedChanged(_param->runtimeParamIndex);
  // TODO }
  // TODO _isOn = getToggleState();
}