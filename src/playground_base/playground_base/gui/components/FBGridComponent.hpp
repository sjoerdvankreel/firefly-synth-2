#pragma once

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

struct FBGridChildAndSpan final
{
  FBGridCell span = {};
  juce::Component* child = {};
};

class FBGridComponent:
public juce::Component,
public IFBHorizontalAutoSize
{
  FBGridType _type;
  juce::Grid _grid = {};
  std::vector<int> _rows;
  std::vector<int> _cols;
  std::vector<FBGridSection> _sections = {};
  std::map<FBGridCell, FBGridChildAndSpan> _cells = {};

  int FixedColWidth(int col, int height) const;
  IFBHorizontalAutoSize* HorizontalAutoSizeAt(int row, int col) const;

public:
  void resized() override;
  void paint(juce::Graphics& g) override;
  int FixedWidth(int height) const override;

  void MarkSection(FBGridSection const& section);
  void Add(int row, int col, juce::Component* child);
  void Add(int row, int col, int rowSpan, int colSpan, juce::Component* child);

  FBGridComponent() = delete;
  FBGridComponent(FBGridType type, int rows, int cols);
  FBGridComponent(FBGridType type, int rows, std::vector<int> const& cols);
  FBGridComponent(FBGridType type, std::vector<int> const& rows, int cols);
  FBGridComponent(FBGridType type, std::vector<int> const& rows, std::vector<int> const& cols);
};