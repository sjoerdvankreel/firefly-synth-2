#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <utility>
#include <cassert>

class IFBParamControl;

class IFBGUIStore
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(IFBGUIStore);
  virtual juce::Component* 
  StoreComponent(std::unique_ptr<juce::Component>&& component) = 0;
  virtual IFBParamControl*
  StoreParamControl(std::unique_ptr<IFBParamControl>&& control) = 0;
};

template <class TComponent, class... Args>
TComponent& FBGUIStoreComponent(IFBGUIStore* store, Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  store->StoreComponent(std::move(component));
  assert(dynamic_cast<IFBParamControl*>(result) == nullptr);
  return *result;
}

template <class TParamControl, class... Args>
TParamControl& FBGUIStoreParamControl(IFBGUIStore* store, Args&&... args)
{
  auto control = std::make_unique<TParamControl>(std::forward<Args>(args)...);
  TParamControl* result = control.get();
  store->StoreParamControl(std::move(control));
  return *result;
}