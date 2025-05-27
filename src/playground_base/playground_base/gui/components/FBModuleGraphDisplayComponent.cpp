#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>
#include <playground_base/gui/components/FBModuleGraphDisplayComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

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
  int maxPointsAllSeries, float absMaxPointAllSeries) const
{
  float y = PointYLocation(points[point], stereo, left, absMaxPointAllSeries);
  float x = PointXLocation(graph, static_cast<float>(point) / maxPointsAllSeries);
  return { x, y };
}

float
FBModuleGraphDisplayComponent::PointXLocation(
  int graph, float pointRelative) const
{
  assert(!std::isnan(pointRelative));
  float graphCount = static_cast<float>(_data->series.size());
  float graphPointRelative = (graph + pointRelative) / graphCount;
  return HalfMarkerSize + graphPointRelative * (getWidth() - MarkerSize);
}

float 
FBModuleGraphDisplayComponent::PointYLocation(
  float pointYValue, bool stereo, 
  bool left, float absMaxPointAllSeries) const
{
  assert(!std::isnan(pointYValue));
  float pointValue = pointYValue / absMaxPointAllSeries;
  if (_data->bipolar)
    pointValue = FBToUnipolar(pointValue);
  if (stereo)
    pointValue = left ? pointValue * 0.5f : 0.5f + pointValue * 0.5f;
  return HalfMarkerSize + (1.0f - pointValue) * (getHeight() - MarkerSize);
}

void
FBModuleGraphDisplayComponent::PaintVerticalIndicator(
  Graphics& g, int graph, int point, 
  int maxPointsAllSeries, float absMaxPointAllSeries)
{
  float dashes[2] = { 4, 2 };
  g.setColour(Colours::white);
  float x = PointXLocation(graph, point / static_cast<float>(maxPointsAllSeries));
  float y0 = PointYLocation(0.0f, false, false, absMaxPointAllSeries);
  float y1 = PointYLocation(absMaxPointAllSeries, false, false, absMaxPointAllSeries);
  g.drawDashedLine(Line<float>(x, y0, x, y1), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintMarker(
  Graphics& g, 
  int graph, std::vector<float> const& points,
  int marker, bool stereo, bool left,
  int maxPointsAllSeries, float absMaxPointAllSeries)
{
  g.setColour(Colours::white);
  auto xy = PointLocation(graph, points, marker, stereo, left, maxPointsAllSeries, absMaxPointAllSeries);
  float x = xy.getX() - HalfMarkerSize;
  float y = xy.getY() - HalfMarkerSize;
  g.fillEllipse(x, y, MarkerSize, MarkerSize);
}

void
FBModuleGraphDisplayComponent::PaintClipBoundaries(
  juce::Graphics& g,
  int graph, bool stereo, bool left,
  float absMaxPointAllSeries)
{
  float dashes[2] = { 4.0, 2.0 };
  float x0 = PointXLocation(graph, 0.0f);
  float x1 = PointXLocation(graph, 1.0f);
  float upperY = PointYLocation(1.0f, stereo, left, absMaxPointAllSeries);
  float lowerY = PointYLocation(_data->bipolar? -1.0f: 0.0f, stereo, left, absMaxPointAllSeries);
  g.setColour(Colours::white);
  g.drawDashedLine(Line<float>(x0, upperY, x1, upperY), dashes, 2);
  g.drawDashedLine(Line<float>(x0, lowerY, x1, lowerY), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintSeries(
  juce::Graphics& g, juce::Colour color,
  int graph, std::vector<float> const& points,
  bool stereo, bool left,
  int maxPointsAllSeries, float absMaxPointAllSeries)
{
  if (points.empty())
    return;

  Path path;
  path.startNewSubPath(PointLocation(graph, points, 0, stereo, left, maxPointsAllSeries, absMaxPointAllSeries));
  for (int i = 1; i < points.size(); i++)
    path.lineTo(PointLocation(graph, points, i, stereo, left, maxPointsAllSeries, absMaxPointAllSeries));
  g.setColour(color);
  g.strokePath(path, PathStrokeType(1.0f));
}

void
FBModuleGraphDisplayComponent::paint(Graphics& g)
{
  for (int s = 0; s < _data->series.size(); s++)
  {
    float absMaxPointAllSeries = 1.0f;
    bool stereo = !_data->series[s].primarySeries.r.empty();
    int maxPointsAllSeries = static_cast<int>(_data->series[s].primarySeries.l.size());
    for (int i = 0; i < _data->series[s].primarySeries.l.size(); i++)
    {
      absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series[s].primarySeries.l[i]));
      if (stereo)
        absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series[s].primarySeries.r[i]));
    }
    for (int i = 0; i < _data->series[s].secondarySeries.size(); i++)
    {
      maxPointsAllSeries = std::max(maxPointsAllSeries, static_cast<int>(_data->series[s].secondarySeries[i].points.l.size()));
      for (int j = 0; j < _data->series[s].secondarySeries[i].points.l.size(); j++)
      {
        absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series[s].secondarySeries[i].points.l[j]));
        if (stereo)
          absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series[s].secondarySeries[i].points.r[j]));
      }
    }

    auto const& vi1 = _data->series[s].verticalIndicators1;
    for (int i = 0; i < vi1.size(); i++)
    {
      assert(!stereo);
      PaintVerticalIndicator(g, s, vi1[i], maxPointsAllSeries, absMaxPointAllSeries);
    }

    g.setColour(Colours::darkgrey);
    auto textBounds = getLocalBounds();
    auto x0 = PointXLocation(s, 0.0f);
    auto x1 = PointXLocation(s, 1.0f);
    textBounds = Rectangle<int>(x0, textBounds.getY(), x1 - x0, textBounds.getHeight());
    g.drawText(_data->series[s].moduleName + " " + _data->series[s].text, textBounds, Justification::centred, false);
    for (int i = 0; i < _data->series[s].secondarySeries.size(); i++)
    {
      int marker = _data->series[s].secondarySeries[i].marker;
      auto const& points = _data->series[s].secondarySeries[i].points;
      PaintSeries(g, Colours::grey, s, points.l, stereo, true, maxPointsAllSeries, absMaxPointAllSeries);
      if (stereo)
        PaintSeries(g, Colours::grey, s, points.r, stereo, false, maxPointsAllSeries, absMaxPointAllSeries);
      if (marker != -1 && _data->drawMarkers)
      {
        assert(!stereo);
        PaintMarker(g, s, points.l, marker, false, true, maxPointsAllSeries, absMaxPointAllSeries);
      }
    }

    PaintSeries(g, Colours::white, s, _data->series[s].primarySeries.l, stereo, true, maxPointsAllSeries, absMaxPointAllSeries);
    if (stereo)
      PaintSeries(g, Colours::white, s, _data->series[s].primarySeries.r, stereo, false, maxPointsAllSeries, absMaxPointAllSeries);
    if (_data->drawMarkers)
      for (int i = 0; i < _data->series[s].primaryMarkers.size(); i++)
      {
        assert(!stereo);
        PaintMarker(g, s, _data->series[s].primarySeries.l, _data->series[s].primaryMarkers[i], false, true, maxPointsAllSeries, absMaxPointAllSeries);
      }

    if (_data->drawClipBoundaries)
    {
      PaintClipBoundaries(g, s, stereo, false, absMaxPointAllSeries);
      if (stereo)
        PaintClipBoundaries(g, s, stereo, true, absMaxPointAllSeries);
    }
  }
}