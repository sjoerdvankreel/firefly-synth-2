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

IFBParamControl*
FBPlugGUI::GetParamControlForIndex(int paramIndex) const
{
  auto iter = _paramIndexToControl.find(paramIndex);
  assert(iter != _paramIndexToControl.end());
  return &dynamic_cast<IFBParamControl&>(*_store[iter->second].get());
}

IFBParamControl*
FBPlugGUI::AddParamControl(int paramIndex, std::unique_ptr<IFBParamControl>&& control)
{
  int controlIndex = (int)_store.size();
  IFBParamControl* controlPtr = std::move(control).release();
  AddComponent(std::unique_ptr<Component>(&dynamic_cast<Component&>(*controlPtr)));
  assert(_paramIndexToControl.find(paramIndex) == _paramIndexToControl.end());
  _paramIndexToControl[paramIndex] = controlIndex;
  return controlPtr;
}