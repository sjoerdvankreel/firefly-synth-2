#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBGUIParamComboBox.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeGUIParam.hpp>

using namespace juce;

FBGUIParamComboBox::
FBGUIParamComboBox(FBPlugGUI* plugGUI, FBRuntimeGUIParam const* param):
FBAutoSizeComboBox(param->static_.ItemsNonRealTime().MakePopupMenu()),
FBGUIParamControl(plugGUI, param)
{
  SetValueNormalizedFromPlug(plugGUI->HostContext()->GetGUIParamNormalized(param->runtimeParamIndex));
}

void
FBGUIParamComboBox::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBGUIParamComboBox::getTooltip()
{
  return _plugGUI->GetTooltipForGUIParam(_param->runtimeParamIndex);
}

void
FBGUIParamComboBox::SetValueNormalizedFromPlug(float normalized)
{
  int plain = _param->static_.NormalizedToAnyDiscreteSlow(normalized);
  setSelectedId(plain + 1, dontSendNotification);
}

void
FBGUIParamComboBox::valueChanged(Value& value)
{
  float normalized = _param->static_.AnyDiscreteToNormalizedSlow(getSelectedId() - 1);
  _plugGUI->HostContext()->SetGUIParamNormalized(_param->runtimeParamIndex, normalized);
  _plugGUI->GUIParamNormalizedChanged(_param->runtimeParamIndex, normalized);
}