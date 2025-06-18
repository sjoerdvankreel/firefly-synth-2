#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeParam.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

using namespace juce;

FBAutoSizeSlider::
FBAutoSizeSlider(FBPlugGUI* plugGUI, Slider::SliderStyle style):
Slider(style, Slider::NoTextBox)
{
  setPopupDisplayEnabled(true, false, plugGUI);
}

int 
FBAutoSizeSlider::FixedWidth(int height) const
{
  FB_ASSERT(getSliderStyle() == SliderStyle::RotaryVerticalDrag || getSliderStyle() == SliderStyle::LinearHorizontal);
  if(getSliderStyle() == SliderStyle::RotaryVerticalDrag)
    return height;
  return 0;
}

FBGUIParamSlider::
FBGUIParamSlider(
  FBPlugGUI* plugGUI,
  FBRuntimeGUIParam const* param, 
  Slider::SliderStyle style):
FBAutoSizeSlider(plugGUI, style),
FBGUIParamControl(plugGUI, param)
{
  if (param->static_.NonRealTime().IsStepped())
    setRange(0.0, 1.0, 1.0 / (param->static_.NonRealTime().ValueCount() - 1.0));
  else
    setRange(0.0, 1.0);
  if (param->static_.type == FBParamType::Linear)
    setSkewFactor(param->static_.Linear().editSkewFactor);
  setDoubleClickReturnValue(true, param->DefaultNormalizedByText());
  SetValueNormalizedFromPlug(plugGUI->HostContext()->GetGUIParamNormalized(param->runtimeParamIndex));
}

void
FBGUIParamSlider::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

void
FBGUIParamSlider::SetValueNormalizedFromPlug(double normalized)
{
  setValue(normalized, dontSendNotification); 
}

String
FBGUIParamSlider::getTooltip()
{
  return _plugGUI->GetTooltipForGUIParam(_param->runtimeParamIndex);
}

void
FBGUIParamSlider::valueChanged()
{
  _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, getValue());
  _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, getValue());
}

String
FBGUIParamSlider::getTextFromValue(double value)
{
  return _param->static_.NormalizedToTextWithUnit(false, value);
}

double
FBGUIParamSlider::getValueFromText(const String& text)
{
  auto parsed = _param->static_.NonRealTime().TextToNormalized(false, text.toStdString());
  return parsed.value_or(_param->DefaultNormalizedByText());
}

FBParamSlider::
FBParamSlider(
  FBPlugGUI* plugGUI,
  FBRuntimeParam const* param, 
  Slider::SliderStyle style):
FBAutoSizeSlider(plugGUI, style),
FBParamControl(plugGUI, param)
{
  if (param->static_.NonRealTime().IsStepped())
    setRange(0.0, 1.0, 1.0 / (param->static_.NonRealTime().ValueCount() - 1.0));
  else
    setRange(0.0, 1.0);
  if (param->static_.type == FBParamType::Linear)
    setSkewFactor(param->static_.Linear().editSkewFactor);
  setDoubleClickReturnValue(true, param->DefaultNormalizedByText());
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
  return _param->static_.NormalizedToTextWithUnit(false, value);
}

double
FBParamSlider::getValueFromText(const String& text)
{
  auto parsed = _param->static_.NonRealTime().TextToNormalized(false, text.toStdString());
  return parsed.value_or(_param->DefaultNormalizedByText());
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