#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
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

struct FBGridChildAndSpan final
{
  FBGridCell span = {};
  juce::Component* child = {};
};

class FBGridComponent:
public juce::Component,
public IFBHorizontalAutoSize
{
  std::vector<int> _rows;
  std::vector<int> _cols;
  std::map<FBGridCell, FBGridChildAndSpan> _cells;

  IFBHorizontalAutoSize* 
  HorizontalAutoSizeAt(int row, int col) const;

public:
  FBGridComponent() = default;
  FBGridComponent(int rows, int cols);
  FBGridComponent(int rows, std::vector<int> const& cols);
  FBGridComponent(std::vector<int> const& rows, int cols);
  FBGridComponent(std::vector<int> const& rows, std::vector<int> const& cols);

  void resized() override;
  int FixedWidth(int height) const override;
  void Add(int row, int col, juce::Component* child);
  void Add(int row, int col, int rowSpan, int colSpan, juce::Component* child);
};