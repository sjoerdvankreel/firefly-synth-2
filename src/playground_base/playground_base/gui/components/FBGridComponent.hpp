#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/gui/shared/FBVerticalAutoSize.hpp>
#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <utility>

enum class FBGridType { Generic, Module };

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
  FBGridType _type;
  juce::Grid _grid = {};
  std::vector<int> _rows;
  std::vector<int> _cols;
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
  FBGridComponent(FBGridType type, int rows, int cols);
  FBGridComponent(FBGridType type, int rows, std::vector<int> const& cols);
  FBGridComponent(FBGridType type, std::vector<int> const& rows, int cols);
  FBGridComponent(FBGridType type, std::vector<int> const& rows, std::vector<int> const& cols);
};