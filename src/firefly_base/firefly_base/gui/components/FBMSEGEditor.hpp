#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

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
public juce::Component
{
  int const _maxPoints;
  int const _maxLengthRatioNum;
  int const _maxLengthRatioDen;
  double const _maxLengthReal;
  int const _gridMinRatioGranularity;

  FBMSEGModel _model = {};
  int _activePointCount = {};
  float _initPointScreenX = {};
  float _initPointScreenY = {};
  std::vector<float> _currentPointsScreenX = {};
  std::vector<float> _currentPointsScreenY = {};
  std::vector<double> _currentSegmentLengths = {};

public:
  FBMSEGEditor(
    int maxPoints,
    int maxLengthRatioNum,
    int maxLengthRatioDen,
    double maxLengthReal,
    int gridMinRatioGranularity);

  void UpdateModel(); // call after mutating Model()
  void paint(juce::Graphics& g) override;
  FBMSEGModel& Model() { return _model; }
  std::function<void(FBMSEGModel const&)> modelChanged = {}; // called in response to GUI
};