#include <firefly_base/gui/components/FBCardComponent.hpp>

static int const CornerSize = 3;

using namespace juce;

FBCardComponent::
FBCardComponent(FBPlugGUI* plugGUI, juce::Component* content):
_plugGUI(plugGUI), _content(content)
{
  addAndMakeVisible(content);
}

void 
FBCardComponent::resized()
{
  _content->setBounds(getLocalBounds().reduced(CornerSize));
  _content->resized();
}

void 
FBCardComponent::paint(Graphics& g)
{
  g.setColour(Colours::green);
  g.fillRoundedRectangle(getLocalBounds().toFloat(), (float)CornerSize);
}

int 
FBCardComponent::FixedHeight() const
{
  auto sizingChild = dynamic_cast<IFBVerticalAutoSize const*>(_content);
  return sizingChild ? (sizingChild->FixedHeight() + 2 * CornerSize) : 0;
}

int 
FBCardComponent::FixedWidth(int height) const
{
  auto sizingChild = dynamic_cast<IFBHorizontalAutoSize const*>(_content);
  return sizingChild ? (sizingChild->FixedWidth(height) + 2 * CornerSize) : 0;
}