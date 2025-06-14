#include <firefly_base/gui/components/FBSectionComponent.hpp>

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
  getChildComponent(0)->setBounds(getLocalBounds().expanded(-2, -2));
  getChildComponent(0)->resized();
}

int
FBSectionComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height - 2 * 2) + 2 * 2;
}