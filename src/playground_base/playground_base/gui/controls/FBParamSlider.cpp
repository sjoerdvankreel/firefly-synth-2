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
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetParamNormalized(param->runtimeParamIndex));
}

void
FBParamSlider::SetValueNormalizedFromHost(float normalized)
{
  setValue(normalized, dontSendNotification);
}

int 
FBParamSlider::FixedWidth(int height) const
{
  assert(getSliderStyle() == SliderStyle::RotaryVerticalDrag);
  return height;
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
  
}

void
FBParamSlider::startedDragging()
{
}

void
FBParamSlider::valueChanged()
{
  _plugGUI->HostContext()->PerformImmediateParamEdit(_param->runtimeParamIndex, (float)getValue());
}

void 
FBParamSlider::mouseUp(MouseEvent const& event)
{
  if (!event.mods.isRightButtonDown())
    return;
  auto menu = _plugGUI->HostContext()->MakeParamContextMenu(_param->runtimeParamIndex);
  if (!menu)
    return;
  auto clicked = [this](int tag) { _plugGUI->HostContext()->ParamContextMenuClicked(_param->runtimeParamIndex, tag); };
  _plugGUI->ShowPopupMenuFor(this, *menu, clicked);
}