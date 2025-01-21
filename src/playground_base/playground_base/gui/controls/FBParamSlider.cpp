#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>

#include <cassert>

using namespace juce;

FBParamSlider::
FBParamSlider(
  FBPlugGUI* plugGUI,
  FBRuntimeParam const* param, 
  Slider::SliderStyle style):
Slider(style, Slider::NoTextBox),
FBParamControl(plugGUI, param)
{
  setRange(0.0, 1.0);
  setPopupDisplayEnabled(true, true, plugGUI);
  setDoubleClickReturnValue(true, param->static_.DefaultNormalizedByText());
  SetValueNormalized(plugGUI->HostContext()->GetParamNormalized(param->runtimeParamIndex));
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

void 
FBParamSlider::stoppedDragging()
{
  _plugGUI->HostContext()->EndParamChange(_param->runtimeParamIndex);
}

void 
FBParamSlider::startedDragging()
{
  _plugGUI->HostContext()->BeginParamChange(_param->runtimeParamIndex);
}

void
FBParamSlider::valueChanged()
{
  _plugGUI->HostContext()->PerformParamEdit(_param->runtimeParamIndex, (float)getValue());
}