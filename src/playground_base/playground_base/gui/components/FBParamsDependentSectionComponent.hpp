#pragma once

#include <playground_base/gui/shared/FBParamsDependent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>

struct FBTopoIndices;
struct FBParamsDependency;

class FBParamsDependentSectionComponent:
public FBSectionComponent,
public FBParamsDependent
{
public:
  void DependenciesChanged(bool outcome) override;
  FBParamsDependentSectionComponent(
    FBPlugGUI* plugGUI, juce::Component* content, 
    FBTopoIndices const& moduleIndices, FBParamsDependency const& dependency);
};