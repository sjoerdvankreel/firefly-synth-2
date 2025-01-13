#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>

using namespace juce;

FBParamSlider::
FBParamSlider(
  FBRuntimeParam const* param,
  IFBHostGUIContext* context,
  Slider::SliderStyle style):
Slider(style, Slider::TextBoxBelow),
_context(context),
_param(param) 
{
  setRange(0.0, 1.0);
  _bounceToHost = false;
  setValue(_context->GetParamNormalized(param->runtimeParamIndex), dontSendNotification);
  _bounceToHost = true;
}

void
FBParamSlider::SetValueNormalized(float normalized)
{
  _bounceToHost = false;
  setValue(normalized, dontSendNotification);
  _bounceToHost = true;
}

void 
FBParamSlider::stoppedDragging()
{
  _context->EndParamChange(_param->runtimeParamIndex);
}

void 
FBParamSlider::startedDragging()
{
  _context->BeginParamChange(_param->runtimeParamIndex);
}

String 
FBParamSlider::getTextFromValue(double value)
{
  return String(_param->static_.NormalizedToText(false, (float)value));
}

void
FBParamSlider::valueChanged()
{
  if (_bounceToHost)
    _context->PerformParamEdit(_param->runtimeParamIndex, (float)getValue());
}

double 
FBParamSlider::getValueFromText(const String& text)
{
  auto parsed = _param->static_.TextToNormalized(false, text.toStdString());
  return parsed.value_or(_param->static_.DefaultNormalizedByText());
}