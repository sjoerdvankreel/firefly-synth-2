#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamsDependent(
  plugGUI, param->topoIndices.module, 
  param->topoIndices.param.slot, param->static_.relevant),
_param(param) {}

void 
FBParamComponent::DependenciesChanged(bool outcome)
{
  dynamic_cast<Component&>(*this).setEnabled(outcome);
}

void
FBParamComponent::UpdateTooltip()
{
  auto tooltip = _plugGUI->GetTooltipForParam(_param->runtimeParamIndex);
  dynamic_cast<SettableTooltipClient&>(*this).setTooltip(tooltip);
}