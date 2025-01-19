#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <utility>
#include <cassert>

class FBGUIStore
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGUIStore);

  template <class TComponent, class... Args>
  TComponent& AddComponent2(Args&&... args);
  template <class TParamControl, class... Args>
  TParamControl& AddParamControl2(FBRuntimeParam const* param, Args&&... args);

  virtual IFBParamControl*
  GetParamControlForIndex(int paramIndex) const = 0;
  virtual juce::Component*
  AddComponent(std::unique_ptr<juce::Component>&& component) = 0;
  virtual IFBParamControl*
  AddParamControl(int index, std::unique_ptr<IFBParamControl>&& control) = 0;
};

template <class TComponent, class... Args>
TComponent& FBGUIStore::AddComponent2(Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  AddComponent(std::move(component));
  assert(dynamic_cast<IFBParamControl*>(result) == nullptr);
  return *result;
}

template <class TParamControl, class... Args>
TParamControl& FBGUIStore::AddParamControl2(FBRuntimeParam const* param, Args&&... args)
{
  auto control = std::make_unique<TParamControl>(param, std::forward<Args>(args)...);
  TParamControl* result = control.get();
  AddParamControl(param->runtimeParamIndex, std::move(control));
  return *result;
}