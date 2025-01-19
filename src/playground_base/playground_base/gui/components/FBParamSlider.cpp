#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>

using namespace juce;

FBParamSlider::
FBParamSlider(
  FBRuntimeParam const* param, Component* root, 
  IFBHostGUIContext* context, Slider::SliderStyle style):
Slider(style, Slider::NoTextBox),
_context(context),
_param(param) 
{
  setRange(0.0, 1.0);
  setPopupDisplayEnabled(true, true, root);
  SetValueNormalized(_context->GetParamNormalized(param->runtimeParamIndex));
}

void
FBParamSlider::SetValueNormalized(float normalized)
{
  setValue(normalized, dontSendNotification);
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

void
FBParamSlider::valueChanged()
{
  _context->PerformParamEdit(_param->runtimeParamIndex, (float)getValue());
}

String 
FBParamSlider::getTextFromValue(double value)
{
  return String(_param->static_.NormalizedToText(false, (float)value));
}

double 
FBParamSlider::getValueFromText(const String& text)
{
  auto parsed = _param->static_.TextToNormalized(false, text.toStdString());
  return parsed.value_or(_param->static_.DefaultNormalizedByText());
}