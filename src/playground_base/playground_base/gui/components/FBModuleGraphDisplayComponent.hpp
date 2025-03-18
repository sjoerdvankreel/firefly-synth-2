#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
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
  void PaintSeries(
    juce::Graphics& g, 
    juce::Colour color, 
    std::vector<float> const& points, 
    bool stereo, bool left,
    int maxPointsAllSeries, float absMaxPointAllSeries);

  void PaintMarker(
    juce::Graphics& g, 
    std::vector<float> const& points, 
    int marker, bool stereo, bool left,
    int maxPointsAllSeries, float absMaxPointAllSeries);
  
  juce::Point<float> PointLocation(
    std::vector<float> const& points,
    int point, bool stereo, bool left, 
    int maxPointsAllSeries, float absMaxPointAllSeries) const;
};