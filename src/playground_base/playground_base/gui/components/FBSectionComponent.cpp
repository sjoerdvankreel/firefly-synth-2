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
FBSectionComponent::FixedWidth() const
{
  // TODO 2 * 2
  return FBAsHorizontalAutoSize(getChildComponent(0))->FixedWidth() + 2 * 2;
}