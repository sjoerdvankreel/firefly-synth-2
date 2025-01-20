#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>

using namespace juce;

FBParamComboBox::
FBParamComboBox(
  FBRuntimeTopo const* topo,
  FBRuntimeParam const* param,
  IFBHostGUIContext* hostContext):
ComboBox(),
FBParamControl(topo, param, hostContext)
{
  for (int i = 0; i < param->static_.ValueCount(); i++)
  {
    float normalized = param->static_.AnyDiscreteToNormalizedSlow(i);
    addItem(param->static_.NormalizedToText(false, normalized), i + 1);
  }
  SetValueNormalized(hostContext->GetParamNormalized(param->runtimeParamIndex));
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
FBParamComboBox::SetValueNormalized(float normalized)
{
  int plain = _param->static_.NormalizedToAnyDiscreteSlow(normalized);
  setSelectedItemIndex(plain, dontSendNotification);
}

void
FBParamComboBox::valueChanged(Value& value)
{
  float normalized = _param->static_.AnyDiscreteToNormalizedSlow(getSelectedItemIndex());
  _hostContext->PerformParamEdit(_param->runtimeParamIndex, normalized);
}