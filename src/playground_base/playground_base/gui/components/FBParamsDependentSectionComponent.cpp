#include <playground_base/base/topo/FBTopoIndices.hpp>
#include <playground_base/base/topo/FBParamsDependency.hpp>
#include <playground_base/gui/components/FBParamsDependentSectionComponent.hpp>

using namespace juce;

FBParamsDependentSectionComponent::
FBParamsDependentSectionComponent(
FBPlugGUI* plugGUI, Component* content, 
FBTopoIndices const& moduleIndices, FBParamsDependency const& dependency):
FBSectionComponent(plugGUI, content),
FBParamsDependent(plugGUI, moduleIndices, 0, dependency) {}

void
FBParamsDependentSectionComponent::DependenciesChanged(bool outcome)
{
  setVisible(outcome);
}