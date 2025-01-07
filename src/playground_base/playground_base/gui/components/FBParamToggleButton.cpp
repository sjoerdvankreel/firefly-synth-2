#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>

using namespace juce;

FBParamToggleButton::
FBParamToggleButton(
  FBRuntimeParam const* param,
  IFBHostGUIContext* context):
ToggleButton(),
_context(context),
_param(param) 
{
  setButtonText(param->longName);
  SetValueNormalized(_context->GetParamNormalized(param->runtimeParamIndex));
}

void
FBParamToggleButton::SetValueNormalized(float normalized)
{
  int plain = _param->static_.boolean.NormalizedToPlain(normalized);
  setToggleState(plain != 0, juce::dontSendNotification);
}

void
FBParamToggleButton::buttonStateChanged()
{
  float normalized = _param->static_.boolean.PlainToNormalized(getToggleState());
  _context->PerformParamEdit(_param->runtimeParamIndex, normalized);
}