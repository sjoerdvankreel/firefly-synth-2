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
  Init,
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
  bool looping = {};
  bool releasing = {};
  double initialY = {};
  int loopStart = {};
  int loopLength = {};
  int releasePoint = {};
  int gridEditRatioGranularity = {};

  FBMSEGXMode xMode = {};
  FBMSEGYMode yMode = {};
  std::vector<FBMSEGPoint> points = {};
};

class FBMSEGEditor :
public juce::Component//,
//public juce::DragAndDropContainer,
//public juce::DragAndDropTarget
{
  FBPlugGUI* const _plugGUI;
  int const _maxPoints;
  int const _maxLengthRatioNum;
  int const _maxLengthRatioDen;
  double const _maxLengthReal;
  int const _gridMinRatioGranularity;

  FBMSEGModel _model = {};
  int _activePointCount = {};
  juce::Point<float> _initPointScreen = {};
  std::vector<double> _currentSegmentLengths = {};
  std::vector<juce::Point<float>> _currentPointsScreen = {};
  std::vector<juce::Point<float>> _currentSlopesScreen = {};

  FBMSEGNearestHitType 
  GetNearestHit(juce::Point<float> const& p, int* index);

public:
  FBMSEGEditor(
    FBPlugGUI* plugGUI,
    int maxPoints,
    int maxLengthRatioNum,
    int maxLengthRatioDen,
    double maxLengthReal,
    int gridMinRatioGranularity);

  void mouseMove(juce::MouseEvent const& event) override;

  void UpdateModel(); // call after mutating Model()
  void paint(juce::Graphics& g) override;
  FBMSEGModel& Model() { return _model; }
  std::function<void(FBMSEGModel const&)> modelChanged = {}; // called in response to GUI
};