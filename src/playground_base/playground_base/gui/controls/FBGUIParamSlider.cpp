#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBGUIParamSlider.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeGUIParam.hpp>

#include <cassert>

using namespace juce;

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
  setDoubleClickReturnValue(true, param->static_.DefaultNormalizedByText());
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
  return parsed.value_or(_param->static_.DefaultNormalizedByText());
}