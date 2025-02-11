#pragma once

#include <playground_base/gui/shared/FBParamsDependent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>

struct FBTopoIndices;
struct FBParamsDependencies;

class FBParamsDependentSectionComponent:
public FBSectionComponent,
public FBParamsDependent
{
public:
  FBParamsDependentSectionComponent(
    FBPlugGUI* plugGUI, juce::Component* content, 
    FBTopoIndices const& moduleIndices, FBParamsDependencies const& dependencies);
};