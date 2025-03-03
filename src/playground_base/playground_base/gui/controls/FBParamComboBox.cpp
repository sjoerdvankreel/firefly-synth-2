#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBParamComboBox::
FBParamComboBox(FBPlugGUI* plugGUI, FBRuntimeParam const* param):  
FBAutoSizeComboBox(param->static_.ItemsNonRealTime().MakePopupMenu()),
FBParamControl(plugGUI, param)
{
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex));
}

void
FBParamComboBox::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

String
FBParamComboBox::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBParamComboBox::SetValueNormalizedFromHost(double normalized)
{
  int plain = _param->static_.NormalizedToAnyDiscreteSlow(normalized);
  setSelectedId(plain + 1, dontSendNotification);
}

void
FBParamComboBox::valueChanged(Value& value)
{
  double normalized = _param->static_.AnyDiscreteToNormalizedSlow(getSelectedId() - 1);
  _plugGUI->HostContext()->PerformImmediateAudioParamEdit(_param->runtimeParamIndex, normalized);
  _plugGUI->AudioParamNormalizedChangedFromUI(_param->runtimeParamIndex, normalized);
}