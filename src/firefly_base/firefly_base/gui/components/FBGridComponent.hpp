#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <utility>

struct FBGridCell final
{
  int row = -1;
  int col = -1;
  bool operator<(FBGridCell const& rhs) const;
};

struct FBGridSection final
{
  FBGridCell pos = {};
  FBGridCell span = {};
};

struct FBGridChildrenAndSpan final
{
  FBGridCell span = {};
  std::vector<juce::Component*> children = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGridChildrenAndSpan);
};

class FBGridComponent:
public juce::Component,
public IFBVerticalAutoSize,
public IFBHorizontalAutoSize
{
  bool const _rowColGap;
  int const _autoSizeCol;
  int const _autoSizeRow;
  std::vector<int> const _rows;
  std::vector<int> const _cols;

  juce::Grid _grid = {};
  std::vector<FBGridSection> _sections = {};
  std::map<FBGridCell, FBGridChildrenAndSpan> _cells = {};

  int FixedRowHeight(int row) const;
  int FixedColWidth(int col, int height) const;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;

  int FixedHeight() const override;
  int FixedWidth(int height) const override;

  void MarkSection(FBGridSection const& section);
  void Add(int row, int col, juce::Component* child);
  void Remove(int row, int col, juce::Component* child);
  void Add(int row, int col, int rowSpan, int colSpan, juce::Component* child);

  FBGridComponent() = delete;
  FBGridComponent(bool rowColGap, int rows, int cols);
  FBGridComponent(bool rowColGap, std::vector<int> const& rows, std::vector<int> const& cols);
  FBGridComponent(bool rowColGap, int autoSizeRow, int autoSizeCol, int rows, int cols);
  FBGridComponent(bool rowColGap, int autoSizeRow, int autoSizeCol, std::vector<int> const& rows, std::vector<int> const& cols);
};