#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamComboBox.hpp>

using namespace juce;

FBParamComboBox::
FBParamComboBox(
  FBRuntimeParam const* param,
  IFBHostGUIContext* context):
ComboBox(),
_context(context),
_param(param) 
{
  for (int i = 0; i < param->static_.ValueCount(); i++)
  {
    float normalized = param->static_.ListOrDiscreteToNormalizedSlow(i);
    addItem(param->static_.NormalizedToText(false, normalized), i + 1);
  }
  SetValueNormalized(_context->GetParamNormalized(param->runtimeParamIndex));
}

void
FBParamComboBox::SetValueNormalized(float normalized)
{
  int plain = _param->static_.NormalizedToListOrDiscreteSlow(normalized);
  setSelectedItemIndex(plain);
}

void
FBParamComboBox::valueChanged(Value& value)
{
  float normalized = _param->static_.ListOrDiscreteToNormalizedSlow(getSelectedItemIndex());
  _context->PerformParamEdit(_param->runtimeParamIndex, normalized);
}