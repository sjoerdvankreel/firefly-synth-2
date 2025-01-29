#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <cassert>

using namespace juce;

bool
FBGridCell::operator<(FBGridCell const& rhs) const
{
  if (row < rhs.row) return true;
  if (row > rhs.row) return false;
  return col < rhs.col;
}

FBGridComponent::
FBGridComponent(FBGridType type, int rows, std::vector<int> const& cols):
FBGridComponent(type, std::vector<int>(rows, 1), cols) {}

FBGridComponent::
FBGridComponent(FBGridType type, std::vector<int> const& rows, int cols):
FBGridComponent(type, rows, std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(FBGridType type, int rows, int cols):
FBGridComponent(type, std::vector<int>(rows, 1), std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(FBGridType type, std::vector<int> const& rows, std::vector<int> const& cols):
Component(), 
_type(type),
_rows(rows),
_cols(cols) {}

void
FBGridComponent::Add(int row, int col, Component* child)
{
  Add(row, col, 1, 1, child);
}

void
FBGridComponent::MarkSection(FBGridSection const& section)
{
  _sections.push_back(section);
}

IFBHorizontalAutoSize*
FBGridComponent::HorizontalAutoSizeAt(int row, int col) const
{
  auto component = _cells.at({ row, col }).child;
  return FBAsHorizontalAutoSize(component);
}

int
FBGridComponent::FixedWidth(int height) const
{
  int result = 0;
  for (int i = 0; i < _cols.size(); i++)
    result += FixedColWidth(i, height);
  auto totalColumnGap = _grid.columnGap.pixels * (_cols.size() - 1);
  return result + static_cast<int>(std::round(totalColumnGap));
}

void
FBGridComponent::Add(int row, int col, int rowSpan, int colSpan, Component* child)
{
  addAndMakeVisible(child);
  FBGridCell cell = { row, col };
  assert(_cells.find(cell) == _cells.end());
  _cells[cell].child = child;
  _cells[cell].span = { rowSpan, colSpan };
}

int 
FBGridComponent::FixedColWidth(int col, int height) const
{
  int result = 0;
  int totalRelativeHeight = 0;
  for (int i = 0; i < _rows.size(); i++)
    totalRelativeHeight += _rows[i];
  auto totalRowGap = std::round((_rows.size() - 1) * _grid.rowGap.pixels);
  int availableGridHeight = height - static_cast<int>(totalRowGap);
  for (int r = 0; r < _rows.size(); r++)
  {
    int rowHeight = (int)std::round(_rows[r] / (float)totalRelativeHeight * availableGridHeight);
    int fixedWidth = HorizontalAutoSizeAt(r, col)->FixedWidth(rowHeight);
    result = std::max(result, fixedWidth);
  }
  return result;
}

void
FBGridComponent::resized()
{
  _grid = {};

  if (_type == FBGridType::Module)
  {
    _grid.rowGap = Grid::Px(2);
    _grid.columnGap = Grid::Px(2); // TODO to topo gui
  }

  for (auto const& e : _cells)
  {
    GridItem item(e.second.child);
    item.row.start = e.first.row + 1;
    item.column.start = e.first.col + 1;
    item.row.end = e.first.row + e.second.span.row + 1;
    item.column.end = e.first.col + e.second.span.col + 1;
    _grid.items.add(item);
  }

  for (int i = 0; i < _rows.size(); i++)
    _grid.templateRows.add(Grid::TrackInfo(Grid::Fr(_rows[i])));
  for (int i = 0; i < _cols.size(); i++)
    if (_cols[i] != 0)
      _grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(_cols[i])));
    else
      _grid.templateColumns.add(Grid::TrackInfo(Grid::Px(FixedColWidth(i, getHeight()))));

  _grid.performLayout(getLocalBounds());
}

void
FBGridComponent::paint(Graphics& g)
{
  int x0, x1, y0, y1;
  x0 = x1 = y0 = y1 = -1;
  g.setColour(Colours::darkgrey);
  for (int i = 0; i < _sections.size(); i++)
  {
    for (int j = 0; j < _grid.items.size(); j++)
    {
      if (_grid.items[j].row.start.getNumber() == _sections[i].pos.row + 1)
        y0 = _grid.items[j].associatedComponent->getY();
      if (_grid.items[j].column.start.getNumber() == _sections[i].pos.col + 1)
        x0 = _grid.items[j].associatedComponent->getX();
      if (_grid.items[j].row.end.getNumber() == _sections[i].pos.row + _sections[i].span.row + 1)
        y1 = _grid.items[j].associatedComponent->getBottom();
      if (_grid.items[j].column.end.getNumber() == _sections[i].pos.col + _sections[i].span.col + 1)
        x1 = _grid.items[j].associatedComponent->getRight();
    }
    g.fillRect(x0, y0, x1 - x0, y1 - y0);
  }
}