#include <playground_base/gui/components/FBGridComponent.hpp>
#include <cassert>

using namespace juce;

FBGridComponent::
FBGridComponent(int rows, int cols):
Component(),
grid()
{
  for(int r = 0; r < rows; r++)
    grid.templateRows.add(Grid::TrackInfo(Grid::Fr(1)));
  for (int c = 0; c < cols; c++)
    grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(1)));
}

void 
FBGridComponent::resized()
{
  grid.performLayout(getLocalBounds());
}

void
FBGridComponent::Add(Component* child)
{
  Add(GridItem(child));
}

void 
FBGridComponent::Add(GridItem const& item)
{
#ifndef NDEBUG
  for (int i = 0; i < getNumChildComponents(); i++)
    assert(getChildComponent(i) != item.associatedComponent);
#endif
  grid.items.add(item);
  addAndMakeVisible(item.associatedComponent);
}