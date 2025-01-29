#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <cassert>

using namespace juce;

FBSectionComponent::
FBSectionComponent(FBPlugGUI* plugGUI, Component* content):
Component()
{
  addAndMakeVisible(content);
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