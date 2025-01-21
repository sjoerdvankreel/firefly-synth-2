#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>

using namespace juce;

FBParamToggleButton::
FBParamToggleButton(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
ToggleButton(),
FBParamControl(plugGUI, param)
{
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetParamNormalized(param->runtimeParamIndex));
  _isOn = getToggleState();
}

int 
FBParamToggleButton::FixedWidth(int height) const
{
  return 24; // TODO;
}

void
FBParamToggleButton::SetValueNormalizedFromHost(float normalized)
{
  int plain = _param->static_.boolean.NormalizedToPlain(normalized);
  setToggleState(plain != 0, dontSendNotification);
  _isOn = getToggleState();
}

void
FBParamToggleButton::buttonStateChanged()
{
  float normalized = _param->static_.boolean.PlainToNormalized(getToggleState());
  int plain = _param->static_.boolean.NormalizedToPlain(normalized);
  if (_isOn != (plain != 0))
  {
    _plugGUI->HostContext()->PerformParamEdit(_param->runtimeParamIndex, normalized);
    _plugGUI->SteppedParamNormalizedChanged(_param->runtimeParamIndex, normalized);
  }
  _isOn = getToggleState();
}