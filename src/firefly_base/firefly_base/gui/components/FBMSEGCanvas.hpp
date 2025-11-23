#pragma once

#include <firefly_base/gui/components/FBMSEGModel.hpp>

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class FBPlugGUI;

class FBMSEGCanvas :
public juce::Component
{
  FBPlugGUI* const _plugGUI;
  std::string const _name;
  int const _maxPoints;
  double const _maxLengthTime;
  FBBarsItem const _maxLengthBars;

  int _dragIndex = -1;
  bool _dragging = false;
  FBMSEGNearestHitType _dragType = {};

  FBMSEGModel _model = {};
  int _activePointCount = {};
  float _totalLengthTime = {};
  float _totalLengthScreen = {}; 
  juce::Point<float> _startPointScreen = {};
  std::vector<float> _currentSnapXScreen = {};
  std::vector<float> _currentSnapYScreen = {};
  std::vector<double> _currentSegmentLengths = {};
  std::vector<juce::Point<float>> _currentPointsScreen = {};
  std::vector<juce::Point<float>> _currentSlopesScreen = {};

  void StopDrag();   
  float SnapToX(float x) const;
  float SnapToY(float y) const;
  juce::Point<float> SnapToXY(juce::Point<float> p) const;
  FBMSEGNearestHitType GetNearestHit(juce::Point<float> const& p, int* index) const;

public:
  FBMSEGCanvas(
    FBPlugGUI* plugGUI,
    std::string const& name,
    int maxPoints,
    double maxLengthTime,
    FBBarsItem maxLengthBars);

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