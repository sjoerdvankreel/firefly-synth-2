#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>

#include <cassert>

using namespace juce;

FBParamSlider::
FBParamSlider(
  FBRuntimeTopo const* topo,
  FBRuntimeParam const* param, 
  IFBHostGUIContext* hostContext,
  Component* root, 
  Slider::SliderStyle style):
Slider(style, Slider::NoTextBox),
FBParamControl(topo, param, hostContext)
{
  setRange(0.0, 1.0);
  setPopupDisplayEnabled(true, true, root);
  setDoubleClickReturnValue(true, param->static_.DefaultNormalizedByText());
  SetValueNormalized(hostContext->GetParamNormalized(param->runtimeParamIndex));
}

int 
FBParamSlider::FixedWidth(int height) const
{
  assert(getSliderStyle() == SliderStyle::Rotary);
  return height;
}

void
FBParamSlider::SetValueNormalized(float normalized)
{
  setValue(normalized, dontSendNotification);
}

void 
FBParamSlider::stoppedDragging()
{
  _hostContext->EndParamChange(_param->runtimeParamIndex);
}

void 
FBParamSlider::startedDragging()
{
  _hostContext->BeginParamChange(_param->runtimeParamIndex);
}

void
FBParamSlider::valueChanged()
{
  _hostContext->PerformParamEdit(_param->runtimeParamIndex, (float)getValue());
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