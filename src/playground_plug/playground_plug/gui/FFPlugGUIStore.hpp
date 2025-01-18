#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <utility>

class IFFPlugGUIStore
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(IFFPlugGUIStore);
  virtual juce::Component* 
  StoreComponent(std::unique_ptr<juce::Component>&& component) = 0;
};

template <class TComponent, class... Args>
TComponent& FFGUIStoreComponent(IFFPlugGUIStore* store, Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  store->StoreComponent(std::move(component));
  return *result;
}