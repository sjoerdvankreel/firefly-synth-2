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
  float y = {};
  float slope = {};
  float lengthReal = {};
  int lengthRatioNum = {};
  int lengthRatioDen = {};
};

struct FBMSEGModel
{
  bool looping = {};
  bool releasing = {};
  float initialY = {};
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
  float const _maxLengthReal;
  int const _gridMinRatioGranularity;

  FBMSEGModel _model = {};

public:
  FBMSEGEditor(
    int maxPoints,
    int maxLengthRatioNum,
    int maxLengthRatioDen,
    float maxLengthReal,
    int gridMinRatioGranularity);

  void paint(juce::Graphics& g) override;
  void ChangeModel(FBMSEGModel const& model);
  std::function<void(FBMSEGModel const&)> modelChanged = {};
};