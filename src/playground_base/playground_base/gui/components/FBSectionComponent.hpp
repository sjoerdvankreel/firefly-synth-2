#pragma once

#include <playground_base/gui/shared/FBParamsDependent.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBTopoIndices;
struct FBParamsDependency;

class FBSectionComponent:
public juce::Component,
public FBParamsDependent,
public IFBHorizontalAutoSize
{
public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  int FixedWidth(int height) const override;
  void DependenciesChanged(bool outcome) override;

  FBSectionComponent(
    FBPlugGUI* plugGUI, juce::Component* content);
  FBSectionComponent(
    FBPlugGUI* plugGUI, FBTopoIndices const& moduleIndices,
    FBParamsDependency const& dependency, juce::Component* content);
};