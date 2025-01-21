#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

static std::vector<int>
RuntimeDependencies(
  FBRuntimeTopo const* topo,
  FBTopoIndices const& staticModuleIndices,
  int staticParamSlot,
  std::vector<int> const& staticParamIndices)
{
  std::vector<int> result;
  for (int i = 0; i < staticParamIndices.size(); i++)
  {
    FBParamTopoIndices indices;
    indices.module = staticModuleIndices;
    indices.param.slot = staticParamSlot;
    indices.param.index = staticParamIndices[i];
    result.push_back(topo->ParamAtTopo(indices)->runtimeParamIndex);
  }
  return result;
}

FBParamsDependent::
FBParamsDependent(
  FBPlugGUI* plugGUI, FBTopoIndices const& moduleIndices,
  int staticParamSlot, FBParamsDependency const& dependency):
_plugGUI(plugGUI),
_evaluations(),
_dependency(dependency),
_runtimeDependencies(
  RuntimeDependencies(
    plugGUI->Topo(), moduleIndices, 
    staticParamSlot, dependency.staticParamIndices)) {}

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