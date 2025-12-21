#include <firefly_base/gui/components/FBMarginComponent.hpp>

static int const MarginSize = 2;

using namespace juce;

FBMarginComponent::
FBMarginComponent(bool l, bool r, bool t, bool b, Component* content):
Component(),
_left(l),
_right(r),
_top(t),
_bottom(b)
{
  addAndMakeVisible(content);
}

int
FBMarginComponent::ExtraX() const
{
  return (_left ? 1 : 0) * MarginSize + (_right ? 1 : 0) * MarginSize;
}

int
FBMarginComponent::ExtraY() const
{
  return (_top ? 1 : 0) * MarginSize + (_bottom ? 1 : 0) * MarginSize;
}

void
FBMarginComponent::resized()
{
  auto bounds = Rectangle<int>(
    getLocalBounds().getX() + _left ? MarginSize : 0,
    getLocalBounds().getY() + _top ? MarginSize : 0,
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