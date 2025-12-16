#include <firefly_base/gui/components/FBModuleComponent.hpp>

using namespace juce;

FBModuleComponent::
FBModuleComponent(int moduleIndex, int moduleSlot, Component* content):
Component(),
_moduleIndex(moduleIndex),
_moduleSlot(moduleSlot)
{
  addAndMakeVisible(content);
}

void
FBModuleComponent::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

int
FBModuleComponent::FixedHeight() const
{
  auto sizingChild = dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedHeight() : 0;
}

int
FBModuleComponent::FixedWidth(int height) const
{
  auto sizingChild = dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedWidth(height) : 0;
}