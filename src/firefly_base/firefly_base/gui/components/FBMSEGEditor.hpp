#pragma once

#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <functional>

class FBPlugGUI;

enum class FBMSEGXMode
{
  Real,
  Ratio
};

enum class FBMSEGYMode
{
  Linear,
  Exponential
};

enum class FBMSEGNearestHitType
{
  None,
  Start,
  Point,
  Slope,
};

struct FBMSEGPoint
{
  double y = {};
  double slope = {};
  double lengthReal = {};
  int lengthRatioNum = {};
  int lengthRatioDen = {};
  
  inline double lengthRatioAsReal() const {
    return (double)lengthRatioNum / (double)lengthRatioDen;
  }
};

struct FBMSEGModel
{
  bool snapX = {};
  bool snapY = {};
  bool enabled = {};
  bool looping = {};
  bool releasing = {};
  double startY = {};
  int loopStart = {};
  int loopLength = {};
  int releasePoint = {};
  int snapXCount = 16;
  int snapYCount = 16;

  FBMSEGXMode xMode = {};
  FBMSEGYMode yMode = {};
  std::vector<FBMSEGPoint> points = {};
};

class FBMSEGEditor :
public juce::Component
{
  FBPlugGUI* const _plugGUI;
  std::string const _name;
  int const _maxPoints;
  int const _maxLengthRatioNum;
  int const _maxLengthRatioDen;
  double const _maxLengthReal;
  std::vector<int> const _snapXCounts;
  std::vector<int> const _snapYCounts;

  int _dragIndex = -1;
  bool _dragging = false;
  FBMSEGNearestHitType _dragType = {};

  FBMSEGModel _model = {};
  int _activePointCount = {};
  float _totalLengthReal = {};
  float _totalLengthScreen = {}; 
  juce::Point<float> _startPointScreen = {};
  std::vector<double> _currentSegmentLengths = {};
  std::vector<juce::Point<float>> _currentPointsScreen = {};
  std::vector<juce::Point<float>> _currentSlopesScreen = {};

  std::unique_ptr<FBGridComponent> _grid = {};
  std::unique_ptr<FBGridComponent> _topGrid = {};
  std::unique_ptr<FBAutoSizeLabel> _snapXLabel = {};
  std::unique_ptr<FBAutoSizeLabel> _snapYLabel = {};
  std::unique_ptr<FBAutoSizeComboBox> _snapXCombo = {};
  std::unique_ptr<FBAutoSizeComboBox> _snapYCombo = {};
  std::unique_ptr<FBAutoSizeToggleButton> _snapXToggle = {};
  std::unique_ptr<FBAutoSizeToggleButton> _snapYToggle = {};

  void StopDrag();   
  FBMSEGNearestHitType 
  GetNearestHit(juce::Point<float> const& p, int* index);

public:
  FBMSEGEditor(
    FBPlugGUI* plugGUI,
    std::string const& name,
    int maxPoints,
    int maxLengthRatioNum,
    int maxLengthRatioDen,
    double maxLengthReal,
    std::vector<int> const& snapXCounts,
    std::vector<int> const& snapYCounts);

  void UpdateModel(); // call after mutating Model()
  FBMSEGModel& Model() { return _model; }

  void resized() override;
  void paint(juce::Graphics& g) override;
  void mouseUp(juce::MouseEvent const& event) override;
  void mouseDrag(juce::MouseEvent const& event) override;
  void mouseDown(juce::MouseEvent const& event) override;
  void mouseMove(juce::MouseEvent const& event) override;
  void mouseExit(juce::MouseEvent const& event) override;
  void mouseDoubleClick(juce::MouseEvent const& event) override;

  std::function<void(FBMSEGModel const&)> modelUpdated = {};
  std::function<std::string(FBMSEGNearestHitType hitType, int index)> getTooltipFor = {};
};