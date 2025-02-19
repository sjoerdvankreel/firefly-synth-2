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

int
FBGridComponent::FixedHeight() const
{
  int result = 0;
  for (int i = 0; i < _rows.size(); i++)
    result += FixedRowHeight(i);
  auto totalRowGap = _grid.rowGap.pixels * (_rows.size() - 1);
  return result + static_cast<int>(std::round(totalRowGap));
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
FBGridComponent::Remove(int row, int col, Component* child)
{
  auto& childrenAtCell = _cells.at({ row, col }).children;
  auto iter = std::find(childrenAtCell.begin(), childrenAtCell.end(), child);
  assert(iter != childrenAtCell.end());
  childrenAtCell.erase(iter);
}

void
FBGridComponent::Add(int row, int col, int rowSpan, int colSpan, Component* child)
{
  addAndMakeVisible(child);
  FBGridCell cell = { row, col };
  auto iter = _cells.find(cell);
  if (iter == _cells.end())
  {
    _cells[cell].children.push_back(child);
    _cells[cell].span = { rowSpan, colSpan };
  }
  else
  {
    _cells[cell].children.push_back(child);
    assert(_cells[cell].span.row == rowSpan);
    assert(_cells[cell].span.col == colSpan);
  }
}

int 
FBGridComponent::FixedRowHeight(int row) const
{
  int result = 0;
  for (int c = 0; c < _cols.size(); c++)
  {
    int fixedCellHeight = 0;
    auto const& sizingChildren = _cells.at({ row, c }).children;
    for (int i = 0; i < sizingChildren.size(); i++)
    {
      auto const& sizingChild = dynamic_cast<IFBVerticalAutoSize&>(*sizingChildren[i]);
      fixedCellHeight = std::max(fixedCellHeight, sizingChild.FixedHeight());
    }
    assert(fixedCellHeight != 0);
    result = std::max(result, fixedCellHeight);
  }
  return result;
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
    int fixedCellWidth = 0;
    auto const& sizingChildren = _cells.at({ r, col }).children;
    int rowHeight = (int)std::round(_rows[r] / (float)totalRelativeHeight * availableGridHeight);
    for (int i = 0; i < sizingChildren.size(); i++)
    {
      auto const& sizingChild = dynamic_cast<IFBHorizontalAutoSize&>(*sizingChildren[i]);
      fixedCellWidth = std::max(fixedCellWidth, sizingChild.FixedWidth(rowHeight));
    }
    assert(fixedCellWidth != 0);
    result = std::max(result, fixedCellWidth);
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
    for(auto child: e.second.children)
    {
      GridItem item(child);
      item.row.start = e.first.row + 1;
      item.column.start = e.first.col + 1;
      item.row.end = e.first.row + e.second.span.row + 1;
      item.column.end = e.first.col + e.second.span.col + 1;
      _grid.items.add(item);
    }

  for (int i = 0; i < _rows.size(); i++)
    if (_rows[i] != 0)
      _grid.templateRows.add(Grid::TrackInfo(Grid::Fr(_rows[i])));
    else
      _grid.templateRows.add(Grid::TrackInfo(Grid::Px(FixedRowHeight(i))));
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
    g.fillRoundedRectangle((float)x0, (float)y0, (float)x1 - x0, (float)y1 - y0, 2.0f); // TODO 2
  }
}