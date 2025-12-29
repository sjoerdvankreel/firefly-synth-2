#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>

static int const MarginX = 1;
static int const MarginY = 2;

using namespace juce;

FBMarginComponent::
FBMarginComponent(bool l, bool r, bool t, bool b, Component* content) :
FBMarginComponent(l, r, t, b, content, false) {}

FBMarginComponent::
FBMarginComponent(bool l, bool r, bool t, bool b, Component* content, bool opaque):
Component(),
_left(l),
_right(r),
_top(t),
_bottom(b),
_opaque(opaque)
{
  addAndMakeVisible(content);
}

int
FBMarginComponent::ExtraX() const
{
  return (_left ? 1 : 0) * MarginX + (_right ? 1 : 0) * MarginX;
}

int
FBMarginComponent::ExtraY() const
{
  return (_top ? 1 : 0) * MarginY + (_bottom ? 1 : 0) * MarginY;
}

void 
FBMarginComponent::paint(Graphics& g)
{
  if (_opaque)
  {
    g.setColour(FBGetLookAndFeel()->FindColorSchemeFor(*this).background);
    g.fillAll();
  }
}

void
FBMarginComponent::resized()
{
  auto bounds = Rectangle<int>(
    getLocalBounds().getX() + _left ? MarginX : 0,
    getLocalBounds().getY() + _top ? MarginY : 0,
    getLocalBounds().getWidth() - ExtraX(),
    getLocalBounds().getHeight() - ExtraY());
  getChildComponent(0)->setBounds(bounds);
  getChildComponent(0)->resized();
}

int
FBMarginComponent::FixedHeight() const
{
  auto sizingChild = dynamic_cast<IFBVerticalAutoSize*>(getChildComponent(0));
  return sizingChild != nullptr ? sizingChild->FixedHeight() + ExtraY() : 0;
}

int
FBMarginComponent::FixedWidth(int height) const
{
  return dynamic_cast<IFBHorizontalAutoSize*>(getChildComponent(0))->FixedWidth(height - ExtraY()) + ExtraX();
}