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
    float pointRelative, bool withPadding) const;

  float PointYLocation(
    float pointYValue, bool stereo, bool left, 
    float absMaxValueAllSeries, bool withPadding) const;

  juce::Point<float> PointLocation(
    std::vector<float> const& points,
    int point, bool stereo, bool left,
    int maxSizeAllSeries, float absMaxValueAllSeries) const;

  void PaintVerticalIndicator(
    juce::Graphics& g, int point, bool primary,
    int maxSizeAllSeries, float absMaxValueAllSeries);

  void PaintMarker(
    juce::Graphics& g, std::vector<float> const& points,
    int marker, bool primary, bool isPointIndicator, bool stereo,
    bool left, int maxSizeAllSeries, float absMaxValueAllSeries);

  void PaintSeries(
    juce::Graphics& g, std::vector<float> const& points,
    bool primary, bool stereo, bool left,
    int maxSizeAllSeries, float absMaxValueAllSeries);
};