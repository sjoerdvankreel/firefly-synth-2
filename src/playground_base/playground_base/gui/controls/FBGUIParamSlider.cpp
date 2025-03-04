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
FBGUIParamSlider::SetValueNormalizedFromPlug(float normalized)
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
  float normalized = (float)getValue();
  _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, normalized);
  _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, normalized);
}

double
FBGUIParamSlider::getValueFromText(const String& text)
{
  auto parsed = _param->static_.TextToNormalized(FBValueTextDisplay::Text, text.toStdString());
  return parsed.value_or(_param->static_.DefaultNormalizedByText());
}

String
FBGUIParamSlider::getTextFromValue(double value)
{
  auto text = _param->static_.NormalizedToText(FBValueTextDisplay::Text, (float)value);
  if (_param->static_.unit.empty())
    return text;
  return text + " " + _param->static_.unit;
}