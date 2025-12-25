#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBThemedComponent.hpp>

using namespace juce;

FBThemedComponent::
FBThemedComponent(int componentId, Component* content):
Component(),
_componentId(componentId),
_content(content)
{
  addAndMakeVisible(content);
}

void 
FBThemedComponent::paint(Graphics& g)
{
  g.fillAll(FBGetLookAndFeel()->FindColorSchemeFor(*this).background);
}

void
FBThemedComponent::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

int
FBThemedComponent::FixedHeight() const
{
  auto sizingChild = dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedHeight() : 0;
}
 
int
FBThemedComponent::FixedWidth(int height) const
{
  auto sizingChild = dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedWidth(height): 0;
}