#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>

using namespace juce;

bool
FBGridCell::operator<(FBGridCell const& rhs) const
{
  if (row < rhs.row) return true;
  if (row > rhs.row) return false;
  return col < rhs.col;
}

FBGridComponent::
FBGridComponent(bool rowColGap, int rows, int cols):
FBGridComponent(rowColGap, -1, -1, std::vector<int>(rows, 1), std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(bool rowColGap, std::vector<int> const& rows, std::vector<int> const& cols):
FBGridComponent(rowColGap, -1, -1, rows, cols) {}

FBGridComponent::
FBGridComponent(bool rowColGap, int autoSizeRow, int autoSizeCol, int rows, int cols):
FBGridComponent(rowColGap, autoSizeRow, autoSizeCol, std::vector<int>(rows, 1), std::vector<int>(cols, 1)) {}

FBGridComponent::
FBGridComponent(bool rowColGap, int autoSizeRow, int autoSizeCol, std::vector<int> const& rows, std::vector<int> const& cols):
FBGridComponent(rowColGap, std::vector<int>(cols.size(), autoSizeRow), std::vector<int>(rows.size(), autoSizeCol), rows, cols) {}

FBGridComponent::
FBGridComponent(
  bool rowColGap, 
  std::vector<int> const& autoSizeRowForCol, 
  std::vector<int> const& autoSizeColForRow, 
  std::vector<int> const& rows, 
  std::vector<int> const& cols):
Component(), 
_rowColGap(rowColGap),
_rows(rows),
_cols(cols),
_autoSizeRowForCol(autoSizeRowForCol),
_autoSizeColForRow(autoSizeColForRow)
{
  FB_ASSERT(autoSizeRowForCol.size() == cols.size());
  FB_ASSERT(autoSizeColForRow.size() == rows.size());
}

void
FBGridComponent::Add(int row, int col, Component* child)
{
  Add(row, col, 1, 1, child);
}

void
FBGridComponent::MarkSection(FBGridSection const& section, FBGridSectionMark mark, float cornerSize, int marginR)
{
  FBGridSectionAndMark sectionAndMark = {};
  sectionAndMark.mark = mark;
  sectionAndMark.marginR = marginR;
  sectionAndMark.section = section;
  sectionAndMark.cornerSize = cornerSize;
  _sectionsAndMarks.push_back(sectionAndMark);
}

int
FBGridComponent::FixedHeight() const
{
  int result = 0;
  for (int i = 0; i < _rows.size(); i++)
    result += FixedRowHeight(i);
  auto totalRowGap = _grid.rowGap.pixels * (_rows.size() - 1);
  return result + static_cast<int>(std::ceil(totalRowGap));
}

int
FBGridComponent::FixedWidth(int height) const
{
  int result = 0;
  for (int i = 0; i < _cols.size(); i++)
    result += FixedColWidth(i, height);
  auto totalColumnGap = _grid.columnGap.pixels * (_cols.size() - 1);
  return result + static_cast<int>(std::ceil(totalColumnGap));
}

void
FBGridComponent::Remove(int row, int col, Component* child)
{
  auto& childrenAtCell = _cells.at({ row, col }).children;
  auto iter = std::find(childrenAtCell.begin(), childrenAtCell.end(), child);
  FB_ASSERT(iter != childrenAtCell.end());
  removeChildComponent(*iter);
  childrenAtCell.erase(iter);
}

void
FBGridComponent::Add(int row, int col, int rowSpan, int colSpan, Component* child)
{
  for (auto const& e : _cells)
    for (auto const& c : e.second.children)
      FB_ASSERT(c != child);

  FB_ASSERT(rowSpan > 0);
  FB_ASSERT(colSpan > 0);
  FB_ASSERT(row + rowSpan <= _rows.size());
  FB_ASSERT(col + colSpan <= _cols.size());

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
    FB_ASSERT(_cells[cell].span.row == rowSpan);
    FB_ASSERT(_cells[cell].span.col == colSpan);
  }
}

int 
FBGridComponent::FixedRowHeight(int row) const
{
  int result = 0;
  for (int c = 0; c < _cols.size(); c++)
  {
    if (_autoSizeColForRow[row] != -1 && _autoSizeColForRow[row] != c)
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
    result = std::max(result, fixedCellHeight);
  }
  if (result == 0)
  {
    if (_rows.size() == 1 && _cols.size() == 1)
      result = getBounds().getHeight();
    else
      FB_ASSERT(false);
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
  auto totalRowGap = std::ceil((_rows.size() - 1) * _grid.rowGap.pixels);
  int availableGridHeight = height - static_cast<int>(totalRowGap);
  for (int i = 0; i < _rows.size(); i++)
    if (_rows[i] == 0)
      availableGridHeight -= FixedRowHeight(i);

  for (int r = 0; r < _rows.size(); r++)
  {
    if (_autoSizeRowForCol[col] != -1 && _autoSizeRowForCol[col] != r)
      continue;
    auto iter = _cells.find({ r, col });
    if (iter == _cells.end())
      continue;
    int fixedCellWidth = 0;
    auto const& sizingChildren = iter->second.children;
    if (_rows[r] == 0)
      rowHeight = FixedRowHeight(r);
    else
      rowHeight = static_cast<int>(std::ceil(_rows[r] / static_cast<float>(totalRelativeHeight) * availableGridHeight));
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

  if (_rowColGap)
  {
    _grid.rowGap = Grid::Px(1);
    _grid.columnGap = Grid::Px(1);
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
  float lineThickness = 2.0f;
  float x0, x1, y0, y1;
  x0 = x1 = y0 = y1 = -1.0f;

  for (int i = 0; i < _sectionsAndMarks.size(); i++)
  {      
    for (int j = 0; j < _grid.items.size(); j++)
    {
      if (_grid.items[j].row.start.getNumber() == _sectionsAndMarks[i].section.pos.row + 1)
        y0 = static_cast<float>(_grid.items[j].associatedComponent->getY());
      if (_grid.items[j].column.start.getNumber() == _sectionsAndMarks[i].section.pos.col + 1)
        x0 = static_cast<float>(_grid.items[j].associatedComponent->getX());
      if (_grid.items[j].row.end.getNumber() == _sectionsAndMarks[i].section.pos.row + _sectionsAndMarks[i].section.span.row + 1)
        y1 = static_cast<float>(_grid.items[j].associatedComponent->getBottom());
      if (_grid.items[j].column.end.getNumber() == _sectionsAndMarks[i].section.pos.col + _sectionsAndMarks[i].section.span.col + 1)
        x1 = static_cast<float>(_grid.items[j].associatedComponent->getRight());
    }
    float subtractR = i == _sectionsAndMarks.size() - 1 ? 0.0f : 2.0f;
    subtractR += _sectionsAndMarks[i].marginR;
    if (_sectionsAndMarks[i].mark == FBGridSectionMark::Background ||
      _sectionsAndMarks[i].mark == FBGridSectionMark::BackgroundAndBorder ||
      _sectionsAndMarks[i].mark == FBGridSectionMark::BackgroundAndAlternate)
    {
      g.setColour(FBGetLookAndFeel()->FindColorSchemeFor(*this).sectionBackground);
      g.fillRoundedRectangle(x0, y0, x1 - x0 - subtractR, y1 - y0, _sectionsAndMarks[i].cornerSize);
    }
    if (_sectionsAndMarks[i].mark == FBGridSectionMark::AlternateAndAlternate)
    {
      g.setColour(FBGetLookAndFeel()->FindColorSchemeFor(*this).primary.darker(2.0f));
      g.fillRoundedRectangle(x0, y0, x1 - x0 - subtractR, y1 - y0, _sectionsAndMarks[i].cornerSize);
    }
    if (_sectionsAndMarks[i].mark == FBGridSectionMark::Alternate ||
      _sectionsAndMarks[i].mark == FBGridSectionMark::BackgroundAndAlternate ||
      _sectionsAndMarks[i].mark == FBGridSectionMark::AlternateAndAlternate)
    {
      g.setColour(FBGetLookAndFeel()->FindColorSchemeFor(*this).primary.withAlpha(0.25f));
      g.drawRoundedRectangle(x0, y0, x1 - x0 - subtractR, y1 - y0, _sectionsAndMarks[i].cornerSize, lineThickness);
    }
    if (_sectionsAndMarks[i].mark == FBGridSectionMark::Border ||
      _sectionsAndMarks[i].mark == FBGridSectionMark::BackgroundAndBorder)
    {
      g.setColour(FBGetLookAndFeel()->FindColorSchemeFor(*this).sectionBorder.withAlpha(0.125f));
      g.drawRoundedRectangle(x0, y0, x1 - x0 - subtractR, y1 - y0, _sectionsAndMarks[i].cornerSize, lineThickness);
    }      
  }
} 