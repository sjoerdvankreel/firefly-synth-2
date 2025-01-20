#pragma once

#include <playground_base/gui/shared/FBHorizontalAutoSize.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <utility>

class FBGridComponent:
public juce::Component,
public IFBHorizontalAutoSize
{
  std::vector<int> _rows;
  std::vector<int> _cols;
  std::map<std::pair<int, int>, juce::Component*> _positions;

  IFBHorizontalAutoSize* 
  HorizontalAutoSizeAt(int row, int col) const;

public:
  void resized() override;
  int FixedWidth() const override;
  void Add(int row, int col, juce::Component* child);

  FBGridComponent() = default;
  FBGridComponent(int rows, int cols);
  FBGridComponent(int rows, std::vector<int> const& cols);
  FBGridComponent(std::vector<int> const& rows, int cols);
  FBGridComponent(std::vector<int> const& rows, std::vector<int> const& cols);
};