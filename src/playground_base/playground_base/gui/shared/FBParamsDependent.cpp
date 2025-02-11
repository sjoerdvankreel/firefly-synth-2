#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

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

struct FBParamsDependentDependency final
{
  std::vector<int> evaluations = {};
  FBParamsDependency dependency = {};
  std::vector<int> const runtimeDependencies = {};
  
  bool Evaluate(FBHostGUIContext const* hostContext);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamsDependentDependency);  
  FBParamsDependentDependency(
    FBRuntimeTopo const* topo, FBTopoIndices const& moduleIndices,
    int staticParamSlot, FBParamsDependency const& dependency);
};

FBParamsDependentDependency::
FBParamsDependentDependency(
  FBRuntimeTopo const* topo, FBTopoIndices const& moduleIndices,
  int staticParamSlot, FBParamsDependency const& dependency) :
evaluations(),
dependency(dependency),
runtimeDependencies(RuntimeDependencies(
  topo, moduleIndices, staticParamSlot, 
  dependency.staticParamIndices)) {}

bool
FBParamsDependentDependency::Evaluate(FBHostGUIContext const* hostContext)
{
  evaluations.clear();
  for (int i = 0; i < runtimeDependencies.size(); i++)
  {
    auto const& param = hostContext->Topo()->params[runtimeDependencies[i]];
    evaluations.push_back(param.static_.NormalizedToAnyDiscreteSlow(
      hostContext->GetParamNormalized(runtimeDependencies[i])));
  }
  return dependency.evaluate(evaluations);
}

FBParamsDependent::
~FBParamsDependent() {}

FBParamsDependent::
FBParamsDependent(
  FBPlugGUI* plugGUI, FBTopoIndices const& moduleIndices,
  int staticParamSlot, FBParamsDependencies const& dependencies):
_plugGUI(plugGUI),
_visible(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), moduleIndices, staticParamSlot, dependencies.visible)),
_enabled(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), moduleIndices, staticParamSlot, dependencies.enabled)) {}

std::vector<int> const& 
FBParamsDependent::RuntimeDependencies(bool visible) const
{
  if (visible)
    return _visible->runtimeDependencies;
  else
    return _enabled->runtimeDependencies;
}

void 
FBParamsDependent::DependenciesChanged(bool visible)
{
  assert(_initialParent != nullptr);
  auto hostContext = _plugGUI->HostContext();
  auto& self = dynamic_cast<Component&>(*this);
  if (!visible)
  {
    self.setEnabled(_enabled->Evaluate(hostContext));
    return;
  }
  _initialParent->removeChildComponent(&self);
  if(_visible->Evaluate(hostContext))
    _initialParent->addChildComponent(&self);
}

void
FBParamsDependent::ParentHierarchyChanged()
{
  auto& self = dynamic_cast<Component&>(*this);
  Component* newParent = self.getParentComponent();
  if (_initialParent == nullptr && newParent != nullptr)
    _initialParent = newParent;
  else if (_initialParent != nullptr && newParent != nullptr && _initialParent != newParent)
    assert(false);
}