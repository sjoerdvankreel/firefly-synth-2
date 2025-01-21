#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

static std::vector<int>
RuntimeDependencies(
  FBRuntimeTopo const* topo,
  std::vector<int> const& staticParamIndices)
{
  std::vector<int> result;
  for (int i = 0; i < staticParamIndices.size(); i++)
  {
    FBParamTopoIndices indices;
    indices.staticParamIndex = param->static_.relevant.staticParamIndices[i];
    indices.staticParamSlot = param->topoIndices.staticParamSlot;
    indices.staticModuleSlot = param->topoIndices.staticModuleSlot;
    indices.staticModuleIndex = param->topoIndices.staticModuleIndex;
    result.push_back(topo->ParamAtTopo(indices)->runtimeParamIndex);
  }
  return result;
}

FBParamsDependent::
FBParamsDependent(FBPlugGUI* plugGUI, FBParamsDependency const& dependency):
_plugGUI(plugGUI),
_evaluations(),
_dependency(dependency),
_runtimeDependencies(VARKEN)


FBParamsDependent::
FBParamsDependent(
  FBPlugGUI* plugGUI, 
  std::vector<int> const& evaluateWhen):
_plugGUI(plugGUI),
_evaluateValues(),
_evaluateWhen(evaluateWhen) {}

bool
FBParamsDependent::Evaluate()
{
  _evaluateValues.clear();
  for (int i = 0; i < _evaluateWhen.size(); i++)
    _evaluateValues.push_back(
      _plugGUI->Topo()->params[_evaluateWhen[i]].static_.NormalizedToAnyDiscreteSlow(
        _plugGUI->HostContext()->GetParamNormalized(_evaluateWhen[i])));
  return Evaluate(_evaluateValues);
}