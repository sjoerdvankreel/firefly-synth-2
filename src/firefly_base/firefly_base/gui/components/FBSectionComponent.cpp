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
  getChildComponent(0)->setBounds(getLocalBounds().reduced(2, 2));
  getChildComponent(0)->resized();
}

int
FBSectionComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height - 4) + 6;
}

FBSubSectionComponent::
FBSubSectionComponent(Component* content, bool topLevel) :
Component(),
_topLevel(topLevel)
{
  addAndMakeVisible(content);
}

void 
FBSubSectionComponent::paint(Graphics& g)
{
  if (_topLevel)
  {
    g.setColour(Colours::black);
    g.fillAll();
  }
  g.setColour(Colour(0xFFA0A0A0));
  g.drawRoundedRectangle(0.0f, 0.0f, (float)getWidth(), (float)getHeight(), 6.0f, 2.0f);
}

void
FBSubSectionComponent::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds().reduced(3, 3));
  getChildComponent(0)->resized();
}

int
FBSubSectionComponent::FixedHeight() const
{
  return dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0))->FixedHeight() + 6;
}

int
FBSubSectionComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height - 6) + 6;
}