#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>

#include <cassert>

using namespace juce;

Component*
FBPlugGUI::AddComponent(std::unique_ptr<Component>&& component)
{
  Component* result = component.get();
  _store.emplace_back(std::move(component));
  return result;
}

FBParamControl*
FBPlugGUI::GetParamControlForIndex(int paramIndex) const
{
  auto iter = _paramIndexToComponent.find(paramIndex);
  assert(iter != _paramIndexToComponent.end());
  return &dynamic_cast<FBParamControl&>(*_store[iter->second].get());
}