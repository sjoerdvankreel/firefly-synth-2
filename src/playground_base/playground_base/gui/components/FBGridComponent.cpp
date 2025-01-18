#include <playground_base/gui/components/FBGridComponent.hpp>
#include <cassert>

using namespace juce;

void 
FBGridComponent::resized()
{
  grid.performLayout(getLocalBounds());
}

void 
FBGridComponent::AddItemAndChild(GridItem const& item)
{
#ifndef NDEBUG
  for (int i = 0; i < getNumChildComponents(); i++)
    assert(getChildComponent(i) != item.associatedComponent);
#endif
  grid.items.add(item);
  addAndMakeVisible(item.associatedComponent);
}