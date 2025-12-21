#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

// todo get rid of it?
FBSectionComponent::
FBSectionComponent(Component* content):
Component()
{
  addAndMakeVisible(content);
}

void
FBSectionComponent::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds().reduced(0, 2));
  getChildComponent(0)->resized();
}

int
FBSectionComponent::FixedHeight() const
{
  auto sizingChild = dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedHeight() + 4 : 0;
}

int
FBSectionComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height - 4);
}

FBSubSectionComponent::
FBSubSectionComponent(bool last, Component* content) :
Component(),
_last(last)
{
  addAndMakeVisible(content);
}

void
FBSubSectionComponent::resized()
{
  if (_last)
    getChildComponent(0)->setBounds(getLocalBounds().reduced(1, 1));
  else
    getChildComponent(0)->setBounds(getLocalBounds().reduced(0, 1).withX(1).withWidth(getLocalBounds().getWidth() - 1));
  getChildComponent(0)->resized();
}

int
FBSubSectionComponent::FixedHeight() const
{
  return dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0))->FixedHeight() + 2;
}

int
FBSubSectionComponent::FixedWidth(int height) const
{
  auto childWidth = dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height - 2);
  return childWidth + (_last ? 2 : 1);
}