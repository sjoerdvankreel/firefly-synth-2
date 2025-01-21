#include <playground_base/base/topo/FBTopoIndices.hpp>
#include <playground_base/base/topo/FBParamsDependency.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>

#include <cassert>

using namespace juce;

FBSectionComponent::
FBSectionComponent(
FBPlugGUI* plugGUI, Component* content):
FBSectionComponent(plugGUI, {}, {}, content) {}

FBSectionComponent::
FBSectionComponent(
FBPlugGUI* plugGUI, FBTopoIndices const& moduleIndices,
FBParamsDependency const& dependency, Component* content):
Component(),
FBParamsDependent(plugGUI, moduleIndices, 0, dependency)
{
  addAndMakeVisible(content);
}

void 
FBSectionComponent::paint(Graphics& g)
{
  // TODO 2
  g.setColour(Colours::darkgrey);
  g.fillRect(getLocalBounds().expanded(-2, -2));
}

void
FBSectionComponent::resized()
{
  // TODO
  getChildComponent(0)->setBounds(getLocalBounds().expanded(-2, -2));
  getChildComponent(0)->resized();
}

int
FBSectionComponent::FixedWidth(int height) const
{
  // TODO 2 * 2
  return FBAsHorizontalAutoSize(getChildComponent(0))->FixedWidth(height - 2 * 2) + 2 * 2;
}