#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

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