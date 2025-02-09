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
}

String
FBParamComboBox::getTooltip()
{
  return _plugGUI->GetTooltipForParam(_param->runtimeParamIndex);
}

int
FBParamComboBox::FixedWidth(int height) const
{
  Font font(12.0f); // TODO
  int maxTextWidth = 0;
  for (int i = 0; i < getNumItems(); i++)
    maxTextWidth = std::max(maxTextWidth, (int)std::ceil(
      TextLayout::getStringWidth(font, getItemText(i))));
  return maxTextWidth + 48; // TODO
}

void
FBParamComboBox::SetValueNormalizedFromHost(float normalized)
{
  int plain = _param->static_.NormalizedToAnyDiscreteSlow(normalized);
  setSelectedItemIndex(plain, dontSendNotification);
}

void
FBParamComboBox::valueChanged(Value& value)
{
  float normalized = _param->static_.AnyDiscreteToNormalizedSlow(getSelectedItemIndex());
  _plugGUI->HostContext()->PerformImmediateParamEdit(_param->runtimeParamIndex, normalized);
  _plugGUI->SetParamNormalizedFromUI(_param->runtimeParamIndex, normalized);
  _plugGUI->SteppedParamNormalizedChanged(_param->runtimeParamIndex);
}