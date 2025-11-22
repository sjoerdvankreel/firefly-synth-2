#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
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
  bool enabled = {};
  bool looping = {};
  bool releasing = {};
  double startY = {};
  int loopStart = {};
  int loopLength = {};
  int releasePoint = {};
  int gridEditRatioGranularity = {};

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
  int const _gridMinRatioGranularity;

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
    int gridMinRatioGranularity);

  void UpdateModel(); // call after mutating Model()
  FBMSEGModel& Model() { return _model; }

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