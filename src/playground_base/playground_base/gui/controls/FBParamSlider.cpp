#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/glue/FBHostContextMenu.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <cassert>

using namespace juce;

FBParamSlider::
FBParamSlider(
  FBPlugGUI* plugGUI,
  FBRuntimeParam const* param, 
  Slider::SliderStyle style):
FBAutoSizeSlider(plugGUI, style),
FBParamControl(plugGUI, param)
{
  setDoubleClickReturnValue(true, param->static_.DefaultNormalizedByText());
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetParamNormalized(param->runtimeParamIndex));
}

void
FBParamSlider::parentHierarchyChanged()
{
  ParentHierarchyChanged();
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

double
FBParamSlider::getValueFromText(const String& text)
{
  auto parsed = _param->static_.TextToNormalized(false, text.toStdString());
  return parsed.value_or(_param->static_.DefaultNormalizedByText());
}

String
FBParamSlider::getTextFromValue(double value)
{
  auto text = _param->static_.NormalizedToText(FBValueTextDisplay::Text, (float)value);
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
  auto const* exchangeState = _plugGUI->HostContext()->ExchangeState();
  auto paramActive = exchangeState->GetParamActiveState(_param);
  if (paramActive != _paramActive)
    repaint();
  _paramActive = paramActive;
}