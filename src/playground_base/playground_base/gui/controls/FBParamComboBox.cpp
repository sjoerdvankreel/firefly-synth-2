#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBParamComboBox::
FBParamComboBox(FBPlugGUI* plugGUI, FBRuntimeParam const* param):  
ComboBox(),
FBParamControl(plugGUI, param)
{
  *getRootMenu() = param->static_.MakePopupMenu();
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetParamNormalized(param->runtimeParamIndex));

  Font font(12.0f); // TODO
  for (int i = 0; i < getNumItems(); i++)
    _maxTextWidth = std::max(_maxTextWidth, (int)std::ceil(
      font.getStringWidthFloat(getItemText(i))));
}

void
FBParamComboBox::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

int
FBParamComboBox::FixedWidth(int height) const
{
  return _maxTextWidth + 48; // TODO
}

String
FBParamComboBox::getTooltip()
{
  return _plugGUI->GetTooltipForParam(_param->runtimeParamIndex);
}

void
FBParamComboBox::SetValueNormalizedFromHost(float normalized)
{
  int plain = _param->static_.NormalizedToAnyDiscreteSlow(normalized);
  setSelectedId(plain + 1, dontSendNotification);
}

void
FBParamComboBox::valueChanged(Value& value)
{
  float normalized = _param->static_.AnyDiscreteToNormalizedSlow(getSelectedId() - 1);
  _plugGUI->HostContext()->PerformImmediateParamEdit(_param->runtimeParamIndex, normalized);
  _plugGUI->SetParamNormalizedFromUI(_param->runtimeParamIndex, normalized);
  _plugGUI->SteppedParamNormalizedChanged(_param->runtimeParamIndex);
}