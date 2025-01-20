#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <vector>
#include <utility>

class FBGridComponent:
public juce::Component
{
  std::vector<int> _rows;
  std::vector<int> _cols;
  std::map<std::pair<int, int>, juce::Component*> _positions;

public:
  void resized() override;
  void Add(int row, int col, juce::Component* child);

  FBGridComponent() = default;
  FBGridComponent(int rows, int cols);
  FBGridComponent(int rows, std::vector<int> const& cols);
  FBGridComponent(std::vector<int> const& rows, int cols);
  FBGridComponent(std::vector<int> const& rows, std::vector<int> const& cols);
};