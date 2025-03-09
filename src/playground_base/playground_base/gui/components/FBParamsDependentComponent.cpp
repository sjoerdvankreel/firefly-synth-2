#include <playground_base/base/topo/runtime/FBTopoIndices.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>
#include <playground_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

FBParamsDependentComponent::
FBParamsDependentComponent(
FBPlugGUI* plugGUI, Component* content, 
FBTopoIndices const& moduleIndices, FBParamsDependencies const& dependencies):
Component(),
FBParamsDependent(plugGUI, moduleIndices, dependencies) 
{
  addAndMakeVisible(content);
}

void 
FBParamsDependentComponent::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

void
FBParamsDependentComponent::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

int
FBParamsDependentComponent::FixedWidth(int height) const
{
  return FBAsHorizontalAutoSize(getChildComponent(0))->FixedWidth(height);
}