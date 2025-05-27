#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

struct FBModuleGraphComponentData;

class FBModuleGraphDisplayComponent final:
public juce::Component
{
  FBModuleGraphComponentData const* const _data;

public:
  void paint(juce::Graphics& g) override;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBModuleGraphDisplayComponent);
  FBModuleGraphDisplayComponent(FBModuleGraphComponentData const* data);

private:
  float PointXLocation(
    int graph, float pointRelative) const;

  float PointYLocation(
    float pointYValue, bool stereo, bool left, 
    float absMaxValueAllSeries) const;

  juce::Point<float> PointLocation(
    int graph, std::vector<float> const& points,
    int point, bool stereo, bool left,
    int maxSizeAllSeries, float absMaxValueAllSeries) const;
  
  void PaintClipBoundaries(
    juce::Graphics& g,
    int graph, bool stereo, bool left,
    float absMaxValueAllSeries);

  void PaintVerticalIndicator(
    juce::Graphics& g, int graph, int point, bool primary,
    int maxSizeAllSeries, float absMaxValueAllSeries);

  void PaintMarker(
    juce::Graphics& g,
    int graph, std::vector<float> const& points,
    int marker, bool stereo, bool left,
    int maxSizeAllSeries, float absMaxValueAllSeries);

  void PaintSeries(
    juce::Graphics& g, juce::Colour color, 
    int graph, std::vector<float> const& points,
    bool stereo, bool left,
    int maxSizeAllSeries, float absMaxValueAllSeries);
};