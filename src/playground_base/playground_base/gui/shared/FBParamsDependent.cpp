#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static std::vector<int>
RuntimeDependencies(
  FBRuntimeTopo const* topo,
  FBTopoIndices const& staticModuleIndices,
  std::vector<int> const& staticParamIndices,
  bool audio)
{
  std::vector<int> result;
  for (int i = 0; i < staticParamIndices.size(); i++)
  {
    FBParamTopoIndices indices;
    indices.module = staticModuleIndices;
    indices.param.slot = 0; // TODO counted params - but difficult with gui/audio params
    indices.param.index = staticParamIndices[i];
    result.push_back(topo->audio.ParamAtTopo(indices)->runtimeParamIndex);
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
    FBTopoIndices const& moduleIndices,
    FBParamsDependency const& dependency,
    bool audio);
};

FBParamsDependentDependency::
FBParamsDependentDependency(
  FBRuntimeTopo const* topo, 
  FBTopoIndices const& moduleIndices,
  FBParamsDependency const& dependency,
  bool audio) :
dependency(dependency),
runtimeDependencies(RuntimeDependencies(
  topo, moduleIndices, dependency.staticParamIndices, audio)) {}

bool
FBParamsDependentDependency::EvaluateGUI(FBHostGUIContext const* hostContext)
{
  if (dependency.evaluate == nullptr)
    return true;
  evaluations.clear();
  for (int i = 0; i < runtimeDependencies.size(); i++)
  {
    auto const& param = hostContext->Topo()->gui.params[runtimeDependencies[i]];
    evaluations.push_back(param.static_.NormalizedToAnyDiscreteSlow(
      hostContext->GetGUIParamNormalized(runtimeDependencies[i])));
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
    evaluations.push_back(param.static_.NormalizedToAnyDiscreteSlow(
      hostContext->GetAudioParamNormalized(runtimeDependencies[i])));
  }
  return dependency.evaluate(evaluations);
}

FBParamsDependent::
~FBParamsDependent() {}

FBParamsDependent::
FBParamsDependent(
  FBPlugGUI* plugGUI, 
  FBTopoIndices const& moduleIndices,
  FBParamsDependencies const& dependencies):
_plugGUI(plugGUI),
_visibleWhenGUI(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), moduleIndices, dependencies.visible.gui, false)),
_enabledWhenGUI(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), moduleIndices, dependencies.enabled.gui, false)),
_visibleWhenAudio(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), moduleIndices, dependencies.visible.audio, true)),
_enabledWhenAudio(std::make_unique<FBParamsDependentDependency>(
  plugGUI->HostContext()->Topo(), moduleIndices, dependencies.enabled.audio, true)) {}

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