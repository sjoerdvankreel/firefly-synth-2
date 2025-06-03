#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static std::vector<int>
RuntimeDependencies(
  FBRuntimeTopo const* topo,
  int slot, bool audio,
  FBTopoIndices const& staticModuleIndices,
  std::vector<int> const& staticParamIndices)
{
  std::vector<int> result;
  for (int i = 0; i < staticParamIndices.size(); i++)
  {
    // Dependency should have slot count 0 or > slot.
    auto const& staticDependency = topo->static_.modules[staticModuleIndices.index].params[staticParamIndices[i]];
    assert(staticDependency.slotCount == 1 || staticDependency.slotCount > slot);
    int dependencySlot = staticDependency.slotCount == 1 ? 0 : slot;

    FBParamTopoIndices indices;
    indices.param.slot = dependencySlot;
    indices.param.index = staticParamIndices[i];
    indices.module = staticModuleIndices;
    if(audio)
      result.push_back(topo->audio.ParamAtTopo(indices)->runtimeParamIndex);
    else
      result.push_back(topo->gui.ParamAtTopo(indices)->runtimeParamIndex);
  }
  return result;
}

struct FBParamsDependentDependency final
{
  std::vector<int> evaluations = {};
  FBParamsDependency dependency;
  std::vector<int> const runtimeDependencies;
  
  bool EvaluateGUI(FBHostGUIContext const* hostContext);
  bool EvaluateAudio(FBHostGUIContext const* hostContext);

  FB_NOCOPY_NOMOVE_NODEFCTOR(FBParamsDependentDependency);  
  FBParamsDependentDependency(
    FBRuntimeTopo const* topo,
    int slot, bool audio,
    FBTopoIndices const& moduleIndices,
    FBParamsDependency const& dependency);
};

FBParamsDependentDependency::
FBParamsDependentDependency(
  FBRuntimeTopo const* topo, 
  int slot, bool audio,
  FBTopoIndices const& moduleIndices,
  FBParamsDependency const& dependency):
dependency(dependency),
runtimeDependencies(RuntimeDependencies(
  topo, slot, audio, moduleIndices, dependency.staticParamIndices)) {}

bool
FBParamsDependentDependency::EvaluateGUI(FBHostGUIContext const* hostContext)
{
  if (dependency.evaluate == nullptr)
    return true;
  evaluations.clear();
  for (int i = 0; i < runtimeDependencies.size(); i++)
  {
    auto const& param = hostContext->Topo()->gui.params[runtimeDependencies[i]];
    double norm = hostContext->GetGUIParamNormalized(runtimeDependencies[i]);
    double plain = param.static_.NonRealTime().NormalizedToPlain(norm);
    evaluations.push_back(static_cast<int>(plain));
  }
  return dependency.evaluate(evaluations);
}

bool
FBParamsDependentDependency::EvaluateAudio(FBHostGUIContext const* hostContext)
{
  if (dependency.evaluate == nullptr)
    return true;
  evaluations.clear();
  for (int i = 0; i < runtimeDependencies.size(); i++)
  {
    auto const& param = hostContext->Topo()->audio.params[runtimeDependencies[i]];
    double norm = hostContext->GetAudioParamNormalized(runtimeDependencies[i]);
    double plain = param.static_.NonRealTime().NormalizedToPlain(norm);
    evaluations.push_back(static_cast<int>(plain));
  }
  return dependency.evaluate(evaluations);
}

FBParamsDependent::
~FBParamsDependent() {}

FBParamsDependent::
FBParamsDependent(
  FBPlugGUI* plugGUI, int slot,
  FBTopoIndices const& moduleIndices,
  FBParamsDependencies const& dependencies):
_plugGUI(plugGUI),
_visibleWhenGUI(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), slot, false, moduleIndices, dependencies.visible.gui)),
_enabledWhenGUI(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), slot, false, moduleIndices, dependencies.enabled.gui)),
_visibleWhenAudio(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), slot, true, moduleIndices, dependencies.visible.audio)),
_enabledWhenAudio(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), slot, true, moduleIndices, dependencies.enabled.audio)) {}

std::vector<int> const& 
FBParamsDependent::RuntimeDependencies(bool audio, bool visible) const
{
  if(audio)
    if (visible)
      return _visibleWhenAudio->runtimeDependencies;
    else
      return _enabledWhenAudio->runtimeDependencies;
  if (visible)
    return _visibleWhenGUI->runtimeDependencies;
  else
    return _enabledWhenGUI->runtimeDependencies;
}

void 
FBParamsDependent::DependenciesChanged(bool visible)
{
  assert(_initialParent != nullptr);
  auto hostContext = _plugGUI->HostContext();
  auto& self = dynamic_cast<Component&>(*this);
  if (!visible)
  {
    bool enabledByGUI = _enabledWhenGUI->EvaluateGUI(hostContext);
    bool enabledByAudio = _enabledWhenAudio->EvaluateAudio(hostContext);
    self.setEnabled(enabledByGUI && enabledByAudio);
    return;
  }
  
  bool isChild = false;
  for (int i = 0; i < _initialParent->getChildren().size(); i++)
    if (_initialParent->getChildren()[i] == &self)
      isChild = true;
  bool visibleByGUI = _visibleWhenGUI->EvaluateGUI(hostContext);
  bool visibleByAudio = _visibleWhenAudio->EvaluateAudio(hostContext);
  bool newIsChild = visibleByAudio && visibleByGUI;
  if (newIsChild == isChild)
    return;
  _initialParent->removeChildComponent(&self);
  if(newIsChild)
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