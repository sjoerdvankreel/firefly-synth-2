#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include <vector>

class FBPlugGUI:
public juce::Component
{
  std::map<int, int> _paramIndexToComponent = {};
  std::vector<std::unique_ptr<juce::Component>> _store = {};

protected:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugGUI);
  juce::Component* 
  AddComponent(std::unique_ptr<juce::Component>&& component);

public:
  template <class TComponent, class... Args>
  TComponent& AddComponent(Args&&... args);
  FBParamControl*
  GetParamControlForIndex(int paramIndex) const;
};

template <class TComponent, class... Args>
TComponent& FBPlugGUI::AddComponent(Args&&... args)
{
  FBParamControl* paramControl = nullptr;
  int componentIndex = (int)_store.size();
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  AddComponent(std::move(component));
  if ((paramControl = dynamic_cast<FBParamControl*>(result)) != nullptr)
    _paramIndexToComponent[paramControl->Param()->runtimeParamIndex] = componentIndex;
  return *result;
}