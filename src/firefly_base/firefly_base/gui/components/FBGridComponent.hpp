#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>

enum class FBGridSectionMark
{
  Background,
  Border,
  Alternate
};

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

struct FBGridSectionAndMark final 
{
  FBGridSection section = {};
  FBGridSectionMark mark = {};
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
  std::vector<int> const _rows;
  std::vector<int> const _cols;
  std::vector<int> const _autoSizeRowForCol;
  std::vector<int> const _autoSizeColForRow;

  juce::Grid _grid = {};
  std::map<FBGridCell, FBGridChildrenAndSpan> _cells = {};
  std::vector<FBGridSectionAndMark> _sectionsAndMarks = {};

  int FixedRowHeight(int row) const;
  int FixedColWidth(int col, int height) const;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;

  int FixedHeight() const override;
  int FixedWidth(int height) const override;

  void Add(int row, int col, juce::Component* child);
  void Remove(int row, int col, juce::Component* child);
  void Add(int row, int col, int rowSpan, int colSpan, juce::Component* child);
  void MarkSection(FBGridSection const& section, FBGridSectionMark mark = FBGridSectionMark::Background);

  FBGridComponent() = delete;
  FBGridComponent(bool rowColGap, int rows, int cols);
  FBGridComponent(bool rowColGap, std::vector<int> const& rows, std::vector<int> const& cols);
  FBGridComponent(bool rowColGap, int autoSizeRow, int autoSizeCol, int rows, int cols);
  FBGridComponent(bool rowColGap, int autoSizeRow, int autoSizeCol, std::vector<int> const& rows, std::vector<int> const& cols);
  FBGridComponent(bool rowColGap, std::vector<int> const& autoSizeRowForCol, std::vector<int> const& autoSizeColForRow, std::vector<int> const& rows, std::vector<int> const& cols);
};