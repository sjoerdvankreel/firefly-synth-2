#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
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
  auto lnf = FBGetLookAndFeelFor(_plugGUI);
  auto const& scheme = lnf->FindColorSchemeFor(*this);
  g.setColour(scheme.sectionBackground);
  g.fillRoundedRectangle(getLocalBounds().toFloat(), (float)CornerSize);
  g.setGradientFill(ColourGradient(scheme.primary.withAlpha(0.1f), 0.0f, 0.0f, scheme.primary.withAlpha(0.0f), 0.0f, (float)getHeight(), false));
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