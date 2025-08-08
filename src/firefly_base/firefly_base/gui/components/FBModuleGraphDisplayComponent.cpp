#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/components/FBModuleGraphComponentData.hpp>
#include <firefly_base/gui/components/FBModuleGraphDisplayComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

static float constexpr Padding = 3.0f;
static float constexpr MarkerSize = 8.0f;
static float constexpr HalfMarkerSize = MarkerSize / 2.0f;

FBModuleGraphDisplayComponent::
FBModuleGraphDisplayComponent(FBModuleGraphComponentData const* data):
Component(),
_data(data) {}

Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  int graph, std::vector<float> const& points,
  int point, bool stereo, bool left,
  int maxSizeAllSeries, float absMaxValueAllSeries) const
{
  point = std::clamp(point, 0, static_cast<int>(points.size()) - 1);
  float y = PointYLocation(points[point], stereo, left, absMaxValueAllSeries, true);
  float x = PointXLocation(graph, static_cast<float>(point) / maxSizeAllSeries, true);
  return { x, y };
}

float
FBModuleGraphDisplayComponent::PointXLocation(
  int graph, float pointRelative, bool withPadding) const
{
  FB_ASSERT(!std::isnan(pointRelative));
  float graphCount = static_cast<float>(_data->graphs.size());
  float gapWidth = 3.0f;
  float gapCount = graphCount - 1.0f;
  float graphWidth = std::floor((getWidth() - gapCount * gapWidth) / graphCount);
  float graphLeft = graph * (graphWidth + gapWidth);
  if (graph == graphCount - 1)
    graphWidth += (getWidth() - gapCount * gapWidth - graphCount * graphWidth);
  if(withPadding)
    return graphLeft + Padding + pointRelative * (graphWidth - 2.0f * Padding);
  return graphLeft + pointRelative * graphWidth;
}

float 
FBModuleGraphDisplayComponent::PointYLocation(
  float pointYValue, bool stereo, 
  bool left, float absMaxValueAllSeries, bool withPadding) const
{
  FB_ASSERT(!std::isnan(pointYValue));
  float pointValue = pointYValue / absMaxValueAllSeries;
  if (_data->bipolar)
    pointValue = FBToUnipolar(pointValue);
  if (stereo)
    pointValue = left ? 0.5f + pointValue * 0.5f: pointValue * 0.5f;
  if(withPadding)
    return HalfMarkerSize + Padding + (1.0f - pointValue) * (getHeight() - MarkerSize - 2.0f * Padding);
  return HalfMarkerSize + (1.0f - pointValue) * (getHeight() - MarkerSize);
}

void
FBModuleGraphDisplayComponent::PaintVerticalIndicator(
  Graphics& g, int graph, int point, bool primary,
  int maxSizeAllSeries, float absMaxValueAllSeries)
{
  float dashes[2] = { 4, 2 };
  g.setColour(Colours::white);
  if (!primary)
    g.setColour(Colours::white.withAlpha(0.5f));
  float x = PointXLocation(graph, point / static_cast<float>(maxSizeAllSeries), true);
  float y0 = PointYLocation(0.0f, false, false, absMaxValueAllSeries, true);
  float y1 = PointYLocation(absMaxValueAllSeries, false, false, absMaxValueAllSeries, true);
  g.drawDashedLine(Line<float>(x, y0, x, y1), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintMarker(
  Graphics& g, 
  int graph, std::vector<float> const& points,
  int marker, bool primary, bool isPointIndicator, bool stereo, 
  bool left, int maxSizeAllSeries, float absMaxValueAllSeries)
{
  g.setColour(Colours::white);
  if (!primary)
    g.setColour(Colours::white.withAlpha(0.5f));
  auto xy = PointLocation(graph, points, marker, stereo, left, maxSizeAllSeries, absMaxValueAllSeries);
  float x = xy.getX() - HalfMarkerSize;
  float y = xy.getY() - HalfMarkerSize;
  if(isPointIndicator)
    g.drawEllipse(x + 2.0f, y + 2.0f, MarkerSize - 4.0f, MarkerSize - 4.0f, 1.0f);
  else
    g.fillEllipse(x, y, MarkerSize, MarkerSize);
}

void
FBModuleGraphDisplayComponent::PaintClipBoundaries(
  juce::Graphics& g,
  int graph, bool stereo, bool left,
  float absMaxValueAllSeries)
{
  float dashes[2] = { 4.0, 2.0 };
  float x0 = PointXLocation(graph, 0.0f, true);
  float x1 = PointXLocation(graph, 1.0f, true);
  float upperY = PointYLocation(1.0f, stereo, left, absMaxValueAllSeries, true);
  float lowerY = PointYLocation(_data->bipolar? -1.0f: 0.0f, stereo, left, absMaxValueAllSeries, true);
  g.setColour(Colours::white);
  g.drawDashedLine(Line<float>(x0, upperY, x1, upperY), dashes, 2);
  g.drawDashedLine(Line<float>(x0, lowerY, x1, lowerY), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintSeries(
  juce::Graphics& g, juce::Colour color,
  int graph, std::vector<float> const& points,
  bool stereo, bool left,
  int maxSizeAllSeries, float absMaxValueAllSeries)
{
  if (points.empty())
    return;

  Path path;
  path.startNewSubPath(PointLocation(graph, points, 0, stereo, left, maxSizeAllSeries, absMaxValueAllSeries));
  for (int i = 1; i < points.size(); i++)
    path.lineTo(PointLocation(graph, points, i, stereo, left, maxSizeAllSeries, absMaxValueAllSeries));
  g.setColour(color);
  g.strokePath(path, PathStrokeType(1.0f));
}

void
FBModuleGraphDisplayComponent::paint(Graphics& g)
{  
  for (int graph = 0; graph < _data->graphs.size(); graph++)
  {
    int maxSizeAllSeries;
    float absMaxValueAllSeries;
    auto& graphData = _data->graphs[graph];
    auto const& primarySeries = graphData.primarySeries;
    auto const& secondarySeries = graphData.secondarySeries;
    bool stereo = !primarySeries.r.empty();
    graphData.GetLimits(maxSizeAllSeries, absMaxValueAllSeries);
    FB_ASSERT(graphData.secondarySeries.size() == 0 || _data->guiRenderType == FBGUIRenderType::Full);

    auto bounds = getLocalBounds();
    auto x0 = static_cast<int>(PointXLocation(graph, 0.0f, false));
    auto x1 = static_cast<int>(PointXLocation(graph, 1.0f, false));
    auto graphBounds = Rectangle<int>(x0, bounds.getY(), x1 - x0, bounds.getHeight());
    g.setColour(Colour(0xFF181818));
    g.fillRoundedRectangle(graphBounds.toFloat(), 6.0f);
    g.setColour(Colour(0xFFA0A0A0));
    g.drawRoundedRectangle(graphBounds.toFloat(), 6.0f, 2.0f);

    if (maxSizeAllSeries != 0)
    {
      auto const& pvi = graphData.primarySeries.verticalIndicators;
      for (int i = 0; i < pvi.size(); i++)
      {
        FB_ASSERT(!stereo);
        PaintVerticalIndicator(g, graph, pvi[i],
          true, maxSizeAllSeries, absMaxValueAllSeries);
      }
      for (int i = 0; i < graphData.secondarySeries.size(); i++)
      {
        auto const& svi = graphData.secondarySeries[i].points.verticalIndicators;
        for (int j = 0; j < svi.size(); j++)
          PaintVerticalIndicator(g, graph, svi[j],
            false, maxSizeAllSeries, absMaxValueAllSeries);
      }
    }

    g.setColour(Colours::darkgrey);
    g.setFont(FBGUIGetFont().withHeight(20.0f));
    g.drawText(graphData.text, graphBounds, Justification::centred, false);

    if (maxSizeAllSeries != 0)
    {
      for (int i = 0; i < secondarySeries.size(); i++)
      {
        int marker = secondarySeries[i].marker;
        auto const& points = secondarySeries[i].points;
        PaintSeries(g, Colours::grey, graph, points.l,
          stereo, true, maxSizeAllSeries, absMaxValueAllSeries);
        if (stereo)
          PaintSeries(g, Colours::grey, graph, points.r,
            stereo, false, maxSizeAllSeries, absMaxValueAllSeries);

        for (int j = 0; j < points.pointIndicators.size(); j++)
          PaintMarker(g, graph, points.l, points.pointIndicators[j],
            false, true, false, true, maxSizeAllSeries, absMaxValueAllSeries);

        if (marker != -1 && _data->drawMarkersSelector != nullptr && _data->drawMarkersSelector(graph))
        {
          FB_ASSERT(!stereo);
          PaintMarker(g, graph, points.l, marker,
            false, false, false, true, maxSizeAllSeries, absMaxValueAllSeries);
        }
      }

      PaintSeries(g, Colours::white, graph, primarySeries.l,
        stereo, true, maxSizeAllSeries, absMaxValueAllSeries);
      if (stereo)
        PaintSeries(g, Colours::white, graph, primarySeries.r,
          stereo, false, maxSizeAllSeries, absMaxValueAllSeries);

      for (int i = 0; i < primarySeries.pointIndicators.size(); i++)
        PaintMarker(g, graph, primarySeries.l, primarySeries.pointIndicators[i],
          true, true, false, true, maxSizeAllSeries, absMaxValueAllSeries);

      FB_ASSERT(graphData.primaryMarkers.size() == 0 || _data->guiRenderType == FBGUIRenderType::Full);
      if (_data->drawMarkersSelector != nullptr && _data->drawMarkersSelector(graph))
        for (int i = 0; i < graphData.primaryMarkers.size(); i++)
        {
          FB_ASSERT(!stereo);
          PaintMarker(g, graph, primarySeries.l, graphData.primaryMarkers[i],
            true, false, false, true, maxSizeAllSeries, absMaxValueAllSeries);
        }

      if (_data->drawClipBoundaries)
      {
        PaintClipBoundaries(g, graph, stereo, false, absMaxValueAllSeries);
        if (stereo)
          PaintClipBoundaries(g, graph, stereo, true, absMaxValueAllSeries);
      }
    }
  }
}