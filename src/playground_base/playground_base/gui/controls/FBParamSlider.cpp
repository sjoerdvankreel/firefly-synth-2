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
  if (param->static_.type == FBParamType::Linear)
    setSkewFactor(param->static_.Linear().editSkewFactor);
  setDoubleClickReturnValue(true, param->static_.DefaultNormalizedByText());
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex));
}

void
FBParamSlider::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

void
FBParamSlider::SetValueNormalizedFromHost(double normalized)
{
  setValue(normalized, dontSendNotification); 
}

String
FBParamSlider::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBParamSlider::mouseUp(MouseEvent const& event)
{
  if (event.mods.isRightButtonDown())
    _plugGUI->ShowHostMenuForAudioParam(_param->runtimeParamIndex);
}

String
FBParamSlider::getTextFromValue(double value)
{
  // TODO all doubles
  return _param->static_.NormalizedToTextWithUnit(FBTextDisplay::Text, value);
}

double
FBParamSlider::getValueFromText(const String& text)
{
  // TODO all doubles
  auto parsed = _param->static_.NonRealTime().TextToNormalized(FBTextDisplay::Text, text.toStdString());
  return parsed.value_or(_param->static_.DefaultNormalizedByText());
}

void
FBParamSlider::stoppedDragging()
{
  _plugGUI->HostContext()->EndAudioParamChange(_param->runtimeParamIndex);
}

void
FBParamSlider::startedDragging()
{
  _plugGUI->HostContext()->BeginAudioParamChange(_param->runtimeParamIndex);
}

void
FBParamSlider::valueChanged()
{
  _plugGUI->HostContext()->PerformAudioParamEdit(_param->runtimeParamIndex, getValue());
  _plugGUI->AudioParamNormalizedChangedFromUI(_param->runtimeParamIndex, getValue());
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