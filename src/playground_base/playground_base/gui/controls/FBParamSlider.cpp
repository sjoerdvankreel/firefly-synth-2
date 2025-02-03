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
  // TODO handle not-per-voice-case
  bool anyExchange = false;
  float minExchange = 1.0f;
  float maxExchange = 0.0f;
  float exchangeValue = 0.0f;
  auto const* exchange = _plugGUI->HostContext()->ExchangeState();
  for(int v = 0; v < FBMaxVoices; v++)
    if (exchange->VoiceState()[v].state == FBVoiceState::Active)
    {
      anyExchange = true;
      exchangeValue = exchange->Params()[_param->runtimeParamIndex].Voice()[v];
      minExchange = std::min(minExchange, exchangeValue);
      maxExchange = std::max(maxExchange, exchangeValue);
    }

  if (!anyExchange || (_minExchange == minExchange && _maxExchange == maxExchange))
    return;

  _minExchange = minExchange;
  _maxExchange = maxExchange;
  repaint();
}