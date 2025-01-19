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

  virtual IFBParamControl*
  GetParamControlForIndex(int paramIndex) const = 0;
  virtual juce::Component*
  AddComponent(std::unique_ptr<juce::Component>&& component) = 0;
  virtual IFBParamControl*
  AddParamControl(int index, std::unique_ptr<IFBParamControl>&& control) = 0;
};

template <class TComponent, class... Args>
TComponent& FBGUIStoreComponent(FBGUIStore* store, Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  store->AddComponent(std::move(component));
  assert(dynamic_cast<IFBParamControl*>(result) == nullptr);
  return *result;
}

template <class TParamControl, class... Args>
TParamControl& FBGUIStoreParamControl(FBGUIStore* store, FBRuntimeParam const* param, Args&&... args)
{
  auto control = std::make_unique<TParamControl>(param, std::forward<Args>(args)...);
  TParamControl* result = control.get();
  store->AddParamControl(param->runtimeParamIndex, std::move(control));
  return *result;
}