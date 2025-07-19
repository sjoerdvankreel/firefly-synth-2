#include <firefly_base/gui/components/FBContentComponent.hpp>

using namespace juce;

void
FBContentComponent::resized()
{
  auto child = getChildComponent(0);
  if (child)
  {
    child->setBounds(getLocalBounds());
    child->resized();
  }
}

int
FBContentComponent::FixedHeight() const
{
  return dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0))->FixedHeight();
}

int
FBContentComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height);
}