#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBEnabledTarget.hpp>

#include <cassert>

using namespace juce;

FBPlugGUI::
FBPlugGUI(FBRuntimeTopo const* topo, IFBHostGUIContext* hostContext):
_topo(topo), _hostContext(hostContext) {}

void
FBPlugGUI::SetParamNormalized(int index, float value)
{
  auto iter = _paramIndexToComponent.find(index);
  assert(iter != _paramIndexToComponent.end());
  dynamic_cast<FBParamControl&>(*_store[iter->second].get()).SetValueNormalized(value);
  for (auto target : _enabledTargets[index])
    dynamic_cast<Component&>(*target).setEnabled(target->Evaluate());
}

Component*
FBPlugGUI::AddComponent(std::unique_ptr<Component>&& component)
{
  FBParamControl* paramControl;
  FBEnabledTarget* enabledTarget;
  int componentIndex = _store.size();
  Component* result = component.get();
  _store.emplace_back(std::move(component));
  if ((paramControl = dynamic_cast<FBParamControl*>(result)) != nullptr)
    _paramIndexToComponent[paramControl->Param()->runtimeParamIndex] = componentIndex;
  if ((enabledTarget = dynamic_cast<FBEnabledTarget*>(result)) != nullptr)
    for (int p : enabledTarget->EvaluateWhen())
      _enabledTargets[p].insert(enabledTarget);
  return result;
}