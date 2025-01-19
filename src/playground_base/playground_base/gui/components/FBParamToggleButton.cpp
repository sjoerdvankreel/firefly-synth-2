#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>

using namespace juce;

FBParamToggleButton::
FBParamToggleButton(
  FBRuntimeParam const* param,
  IFBHostGUIContext* context):
ToggleButton(),
FBParamControl(param),
_context(context)
{
  SetValueNormalized(_context->GetParamNormalized(param->runtimeParamIndex));
  _isOn = getToggleState();
}

void
FBParamToggleButton::SetValueNormalized(float normalized)
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
  if(_isOn != (plain != 0))
    _context->PerformParamEdit(_param->runtimeParamIndex, normalized);
  _isOn = getToggleState();
}