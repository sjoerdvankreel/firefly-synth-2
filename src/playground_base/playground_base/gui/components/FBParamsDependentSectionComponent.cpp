#include <playground_base/base/topo/runtime/FBTopoIndices.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>
#include <playground_base/gui/components/FBParamsDependentSectionComponent.hpp>

using namespace juce;

FBParamsDependentSectionComponent::
FBParamsDependentSectionComponent(
FBPlugGUI* plugGUI, Component* content, 
FBTopoIndices const& moduleIndices, FBParamsDependencies const& dependencies):
FBSectionComponent(plugGUI, content),
FBParamsDependent(plugGUI, moduleIndices, 0, dependencies) {}

void 
FBParamsDependentSectionComponent::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}