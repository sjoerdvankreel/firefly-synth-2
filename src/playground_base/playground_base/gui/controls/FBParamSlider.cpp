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

String
FBParamSlider::getTooltip()
{
  return _plugGUI->GetTooltipForParam(_param->runtimeParamIndex);
}

void
FBParamSlider::SetValueNormalizedFromHost(float normalized)
{
  setValue(normalized, dontSendNotification); 
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
}

void
FBParamSlider::UpdateExchangeState()
{
  float exchangeValue = 0.0f;
  bool newExchangeActive = false;
  float newMinExchangeValue = 1.0f;
  float newMaxExchangeValue = 0.0f;
  
  auto const* exchangeState = _plugGUI->HostContext()->ExchangeState();
  auto const& paramExchange = exchangeState->Params()[_param->runtimeParamIndex];
  auto const& activeExchange = exchangeState->Active()[_param->runtimeModuleIndex];

  if (paramExchange.IsGlobal())
    if (*activeExchange.Global())
    {
      newExchangeActive = true;
      newMinExchangeValue = std::min(newMinExchangeValue, *paramExchange.Global());
      newMaxExchangeValue = std::max(newMaxExchangeValue, *paramExchange.Global());
    }
  if(!paramExchange.IsGlobal())
    for (int v = 0; v < FBMaxVoices; v++)
      if (*activeExchange.Voice()[v])
      {
        newExchangeActive = true;
        newMinExchangeValue = std::min(newMinExchangeValue, paramExchange.Voice()[v]);
        newMaxExchangeValue = std::max(newMaxExchangeValue, paramExchange.Voice()[v]);
      }

  if (_exchangeActive != newExchangeActive || 
    _minExchangeValue != newMinExchangeValue || 
    _maxExchangeValue != newMaxExchangeValue)
    repaint();
  _exchangeActive = newExchangeActive;
  _minExchangeValue = newMinExchangeValue;
  _maxExchangeValue = newMaxExchangeValue;
}