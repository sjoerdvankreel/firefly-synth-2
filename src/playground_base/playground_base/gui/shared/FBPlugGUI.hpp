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
  std::map<int, int> _paramIndexToControl = {};
  std::vector<std::unique_ptr<juce::Component>> _store = {};

protected:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBPlugGUI);

  juce::Component* 
  AddComponent(std::unique_ptr<juce::Component>&& component);
  IFBParamControl* 
  AddParamControl(int index, std::unique_ptr<IFBParamControl>&& control);

public:
  IFBParamControl* 
  GetParamControlForIndex(int paramIndex) const;

  template <class TComponent, class... Args>
  TComponent& AddComponent(Args&&... args);
  template <class TParamControl, class... Args>
  TParamControl& AddParamControl(FBRuntimeParam const* param, Args&&... args);
};

template <class TComponent, class... Args>
TComponent& FBPlugGUI::AddComponent(Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  AddComponent(std::move(component));
  assert(dynamic_cast<IFBParamControl*>(result) == nullptr);
  return *result;
}

template <class TParamControl, class... Args>
TParamControl& FBPlugGUI::AddParamControl(FBRuntimeParam const* param, Args&&... args)
{
  auto control = std::make_unique<TParamControl>(param, std::forward<Args>(args)...);
  TParamControl* result = control.get();
  AddParamControl(param->runtimeParamIndex, std::move(control));
  return *result;
}