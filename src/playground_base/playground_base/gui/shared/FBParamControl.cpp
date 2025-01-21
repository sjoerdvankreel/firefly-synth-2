#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>

static std::vector<int>
EvaluateWhen(
  FBRuntimeTopo const* topo, 
  FBRuntimeParam const* param)
{
  std::vector<int> result;
  for (int i = 0; i < param->static_.enabledWhen.size(); i++)
  {
    FBParamTopoIndices indices;
    indices.staticParamIndex = param->static_.enabledWhen[i];
    indices.staticParamSlot = param->topoIndices.staticParamSlot;
    indices.staticModuleSlot = param->topoIndices.staticModuleSlot;
    indices.staticModuleIndex = param->topoIndices.staticModuleIndex;
    result.push_back(topo->ParamAtTopo(indices)->runtimeParamIndex);
  }
  return result;
}

FBParamControl::
FBParamControl(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBEnabledTarget(plugGUI, ::EvaluateWhen(plugGUI->Topo(), param)),
_param(param) {}

bool 
FBParamControl::Evaluate(std::vector<int> const& vals) const
{
  return _param->static_.enabledSelector(vals);
}