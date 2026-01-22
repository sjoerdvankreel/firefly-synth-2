#pragma once

#include <firefly_base/gui/shared/FBTheme.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBModuleGraphComponentData;

class FBModuleGraphDisplayComponent final:
public juce::Component
{
  FBPlugGUI* const _plugGUI;
  FBModuleGraphComponentData const* const _data;
  int const _graphIndex;

  FBColorScheme const& FindColorSchemeFor(
    int moduleIndex, int moduleSlot) const;

public:
  void paint(juce::Graphics& g) override;
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBModuleGraphDisplayComponent);
  FBModuleGraphDisplayComponent(FBPlugGUI* plugGUI, FBModuleGraphComponentData const* data, int graphIndex);

private:
  float PointXLocation(
    bool primarySeries, int secondaryIndex,
    float pointRelative, bool withPadding) const;

  float PointYLocation(
    bool primarySeries, int secondaryIndex,
    float pointYValue, bool stereo, bool left, 
    float absMaxValueAllSeries, bool withPadding) const;

  juce::Point<float> PointLocation(
    bool primarySeries, int secondaryIndex,
    std::vector<float> const& points,
    int point, bool stereo, bool left,
    int maxSizeAllSeries, float absMaxValueAllSeries) const;

  void PaintVerticalIndicator(
    juce::Graphics& g, 
    bool primarySeries, int secondaryIndex, int point,
    int maxSizeAllSeries, float absMaxValueAllSeries);

  void PaintMarker(
    juce::Graphics& g, 
    bool primarySeries, int secondaryIndex,
    std::vector<float> const& points,
    int marker, bool isPointIndicator, bool stereo,
    bool left, int maxSizeAllSeries, float absMaxValueAllSeries);

  void PaintSeries(
    juce::Graphics& g, 
    bool primarySeries, int secondaryIndex,
    std::vector<float> const& points,
    bool stereo, bool left,
    int maxSizeAllSeries, float absMaxValueAllSeries);
};