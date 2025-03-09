#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <cassert>

using namespace juce;

FBSectionComponent::
FBSectionComponent(Component* content):
Component()
{
  addAndMakeVisible(content);
}

void
FBSectionComponent::resized()
{
  // TODO part of topo gui
  getChildComponent(0)->setBounds(getLocalBounds().expanded(-2, -2));
  getChildComponent(0)->resized();
}

int
FBSectionComponent::FixedWidth(int height) const
{
  // TODO part of topo gui
  return FBAsHorizontalAutoSize(getChildComponent(0))->FixedWidth(height - 2 * 2) + 2 * 2;
}