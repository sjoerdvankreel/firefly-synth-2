#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/glue/FBHostContextMenu.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/base/state/FBExchangeStateContainer.hpp>

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
  setPopupDisplayEnabled(true, false, plugGUI);
  setDoubleClickReturnValue(true, param->static_.DefaultNormalizedByText());
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetParamNormalized(param->runtimeParamIndex));
}

void
FBParamSlider::SetValueNormalizedFromHost(float normalized)
{
  setValue(normalized, dontSendNotification); 
  UpdateTooltip();
}

void
FBParamSlider::mouseUp(MouseEvent const& event)
{
  if (event.mods.isRightButtonDown())
    _plugGUI->ShowHostMenuForParam(_param->runtimeParamIndex);
}

int 
FBParamSlider::FixedWidth(int height) const
{
  assert(getSliderStyle() == SliderStyle::RotaryVerticalDrag);
  return height;
}

void
FBParamSlider::ReceivedNewExchangeValue(float exchangeValue)
{
  _exchangeValueReceived = true;
  _minExchangeValue = std::min(_minExchangeValue, exchangeValue);
  _maxExchangeValue = std::max(_maxExchangeValue, exchangeValue);
}

double
FBParamSlider::getValueFromText(const String& text)
{
  auto parsed = _param->static_.TextToNormalized(false, text.toStdString());
  return parsed.value_or(_param->static_.DefaultNormalizedByText());
}

String
FBParamSlider::getTextFromValue(double value)
{
  auto text = _param->static_.NormalizedToText(FBTextDisplay::Text, (float)value);
  if (_param->static_.unit.empty())
    return text;
  return text + " " + _param->static_.unit;
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
  float normalized = (float)getValue();
  _plugGUI->HostContext()->PerformParamEdit(_param->runtimeParamIndex, normalized);
  _plugGUI->SetParamNormalizedFromUI(_param->runtimeParamIndex, normalized);
  UpdateTooltip();
}

void
FBParamSlider::UpdateExchangeState()
{
  float exchangeValue = 0.0f;
  _minExchangeValue = 1.0f;
  _maxExchangeValue = 0.0f;
  _exchangeValueReceived = false;

  auto const* exchange = _plugGUI->HostContext()->ExchangeState();
  if (exchange->Params()[_param->runtimeParamIndex].IsGlobal())
    ReceivedNewExchangeValue(*exchange->Params()[_param->runtimeParamIndex].Global());
  else
    for(int v = 0; v < FBMaxVoices; v++)
      if (exchange->VoiceState()[v].state == FBVoiceState::Active)
        ReceivedNewExchangeValue(exchange->Params()[_param->runtimeParamIndex].Voice()[v]);

  if (_exchangeValueReceived)
    repaint();
}