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
FBGridComponent(FBGridType type, int rows, int cols):
FBGridComponent(type, -1, -1, std::vector<int>(rows, 1), std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(FBGridType type, std::vector<int> const& rows, std::vector<int> const& cols):
FBGridComponent(type, -1, -1, rows, cols) {}

FBGridComponent::
FBGridComponent(FBGridType type, int autoSizeRow, int autoSizeCol, int rows, int cols):
FBGridComponent(type, autoSizeRow, autoSizeCol, std::vector<int>(rows, 1), std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(FBGridType type, int autoSizeRow, int autoSizeCol, std::vector<int> const& rows, std::vector<int> const& cols):
Component(), 
_type(type),
_autoSizeRow(autoSizeRow),
_autoSizeCol(autoSizeCol),
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
  removeChildComponent(*iter);
  childrenAtCell.erase(iter);
}

void
FBGridComponent::Add(int row, int col, int rowSpan, int colSpan, Component* child)
{
  for (auto const& e : _cells)
    for (auto const& c : e.second.children)
      assert(c != child);

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
    if (_autoSizeCol != -1 && _autoSizeCol != c)
      continue;
    auto iter = _cells.find({ row, c });
    if (iter == _cells.end())
      continue;
    int fixedCellHeight = 0;
    auto const& sizingChildren = iter->second.children;
    for (int i = 0; i < sizingChildren.size(); i++)
    {
      auto sizingChild = dynamic_cast<IFBVerticalAutoSize*>(sizingChildren[i]);
      if(sizingChild != nullptr)
        fixedCellHeight = std::max(fixedCellHeight, sizingChild->FixedHeight());
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
  int rowHeight = 0;
  int totalRelativeHeight = 0;
  for (int i = 0; i < _rows.size(); i++)
    totalRelativeHeight += _rows[i];
  auto totalRowGap = std::round((_rows.size() - 1) * _grid.rowGap.pixels);
  int availableGridHeight = height - static_cast<int>(totalRowGap);
  for (int i = 0; i < _rows.size(); i++)
    if (_rows[i] == 0)
      availableGridHeight -= FixedRowHeight(i);

  for (int r = 0; r < _rows.size(); r++)
  {
    if (_autoSizeRow != -1 && _autoSizeRow != r)
      continue;
    auto iter = _cells.find({ r, col });
    if (iter == _cells.end())
      continue;
    int fixedCellWidth = 0;
    auto const& sizingChildren = iter->second.children;
    if (_rows[r] == 0)
      rowHeight = FixedRowHeight(r);
    else
      rowHeight = static_cast<int>(std::round(_rows[r] / static_cast<float>(totalRelativeHeight) * availableGridHeight));
    for (int i = 0; i < sizingChildren.size(); i++)
    {
      auto sizingChild = dynamic_cast<IFBHorizontalAutoSize*>(sizingChildren[i]);
      if(sizingChild != nullptr)
        fixedCellWidth = std::max(fixedCellWidth, sizingChild->FixedWidth(rowHeight));
    }
    result = std::max(result, fixedCellWidth);
  }
  return result;
}

void
FBGridComponent::resized()
{
  _grid = {};

  if (getWidth() == 0 || getHeight() == 0)
    return;

  if (_type == FBGridType::Module)
  {
    _grid.rowGap = Grid::Px(2);
    _grid.columnGap = Grid::Px(2);
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
  float x0, x1, y0, y1;
  x0 = x1 = y0 = y1 = -1.0f;
  g.setColour(Colour(0xFF181818));
  for (int i = 0; i < _sections.size(); i++)
  {
    for (int j = 0; j < _grid.items.size(); j++)
    {
      if (_grid.items[j].row.start.getNumber() == _sections[i].pos.row + 1)
        y0 = static_cast<float>(_grid.items[j].associatedComponent->getY());
      if (_grid.items[j].column.start.getNumber() == _sections[i].pos.col + 1)
        x0 = static_cast<float>(_grid.items[j].associatedComponent->getX());
      if (_grid.items[j].row.end.getNumber() == _sections[i].pos.row + _sections[i].span.row + 1)
        y1 = static_cast<float>(_grid.items[j].associatedComponent->getBottom());
      if (_grid.items[j].column.end.getNumber() == _sections[i].pos.col + _sections[i].span.col + 1)
        x1 = static_cast<float>(_grid.items[j].associatedComponent->getRight());
    }
    g.fillRoundedRectangle(x0, y0, x1 - x0, y1 - y0, 2.0f); // TODO 2
  }
}