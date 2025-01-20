#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <cassert>

using namespace juce;

FBGridComponent::
FBGridComponent(int rows, std::vector<int> const& cols):
FBGridComponent(std::vector<int>(rows, 1), cols) {}

FBGridComponent::
FBGridComponent(std::vector<int> const& rows, int cols):
FBGridComponent(rows, std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(int rows, int cols):
FBGridComponent(std::vector<int>(rows, 1), std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(std::vector<int> const& rows, std::vector<int> const& cols):
Component(),
_rows(rows),
_cols(cols),
_positions() {}

int
FBGridComponent::FixedWidth() const
{
  // TODO all rows
  int result = 0;
  for (int i = 0; i < _cols.size(); i++)
    result += HorizontalAutoSizeAt(0, i)->FixedWidth();
  return result;
}

void
FBGridComponent::Add(int row, int col, Component* child)
{
  addAndMakeVisible(child);
  auto position = std::make_pair(row, col);
  assert(_positions.find(position) == _positions.end());
  _positions[position] = child;
}

IFBHorizontalAutoSize*
FBGridComponent::HorizontalAutoSizeAt(int row, int col) const
{
  auto position = std::make_pair(row, col);
  auto component = _positions.at(position);
  return FBAsHorizontalAutoSize(component);
}

void
FBGridComponent::resized()
{
  Grid grid;
  for (auto const& e : _positions)
  {
    // TODO span
    GridItem item(e.second);
    item.row.end = e.first.first + 2;
    item.row.start = e.first.first + 1;
    item.column.end = e.first.second + 2;
    item.column.start = e.first.second + 1;
    grid.items.add(item);
  }

  // TODO rows
  // TODO this assumes first row is autosize
  std::vector<int> absoluteOrRelative = _cols;
  for (int i = 0; i < _cols.size(); i++)
    if (_cols[i] == 0)
      absoluteOrRelative[i] = HorizontalAutoSizeAt(0, i)->FixedWidth();
  for (int i = 0; i < _rows.size(); i++)
    grid.templateRows.add(Grid::TrackInfo(Grid::Fr(1)));
  for (int i = 0; i < absoluteOrRelative.size(); i++)
    if (_cols[i] == 0)
      grid.templateColumns.add(Grid::TrackInfo(Grid::Px(absoluteOrRelative[i])));
    else
      grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(_cols[i])));  
  grid.performLayout(getLocalBounds());
}