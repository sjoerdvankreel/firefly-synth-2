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
  ::RuntimeDependencies(
    plugGUI->HostContext()->Topo(), moduleIndices, 
    staticParamSlot, dependency.staticParamIndices)) {}

void 
FBParamsDependent::DependenciesChanged()
{
  _evaluations.clear();
  for (int i = 0; i < _runtimeDependencies.size(); i++)
    _evaluations.push_back(
      _plugGUI->HostContext()->Topo()->params[_runtimeDependencies[i]].static_.NormalizedToAnyDiscreteSlow(
        _plugGUI->HostContext()->GetParamNormalized(_runtimeDependencies[i])));
  DependenciesChanged(_dependency.evaluate(_evaluations));
}