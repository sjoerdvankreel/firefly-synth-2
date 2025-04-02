#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>
#include <playground_base/gui/components/FBModuleGraphDisplayComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

static float constexpr MarkerSize = 8.0f; // TODO to topo gui or config header
static float constexpr HalfMarkerSize = MarkerSize / 2.0f;

FBModuleGraphDisplayComponent::
FBModuleGraphDisplayComponent(FBModuleGraphComponentData const* data):
Component(),
_data(data) {}

Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  std::vector<float> const& points,
  int point, bool stereo, bool left,
  int maxPointsAllSeries, float absMaxPointAllSeries) const
{
  float y = PointYLocation(points[point], stereo, left, absMaxPointAllSeries);
  float x = PointXLocation(static_cast<float>(point) / maxPointsAllSeries);
  return { x, y };
}

float
FBModuleGraphDisplayComponent::PointXLocation(
  float pointRelative) const
{
  return HalfMarkerSize + pointRelative * (getWidth() - MarkerSize);
}

float 
FBModuleGraphDisplayComponent::PointYLocation(
  float pointYValue, bool stereo, 
  bool left, float absMaxPointAllSeries) const
{
  float pointValue = pointYValue / absMaxPointAllSeries;
  if (_data->bipolar)
    pointValue = FBToUnipolar(pointValue);
  if (stereo)
    pointValue = left ? pointValue * 0.5f : 0.5f + pointValue * 0.5f;
  return HalfMarkerSize + (1.0f - pointValue) * (getHeight() - MarkerSize);
}

void
FBModuleGraphDisplayComponent::PaintMarker(
  Graphics& g, 
  std::vector<float> const& points,
  int marker, bool stereo, bool left,
  int maxPointsAllSeries, float absMaxPointAllSeries)
{
  g.setColour(Colours::white);
  auto xy = PointLocation(points, marker, stereo, left, maxPointsAllSeries, absMaxPointAllSeries);
  float x = xy.getX() - HalfMarkerSize;
  float y = xy.getY() - HalfMarkerSize;
  g.fillEllipse(x, y, MarkerSize, MarkerSize);
}

void
FBModuleGraphDisplayComponent::PaintClipBoundaries(
  juce::Graphics& g,
  bool stereo, bool left,
  float absMaxPointAllSeries)
{
  float dashes[2] = { 4.0, 2.0 };
  float x0 = PointXLocation(0.0f);
  float x1 = PointXLocation(1.0f);
  float upperY = PointYLocation(1.0f, stereo, left, absMaxPointAllSeries);
  float lowerY = PointYLocation(_data->bipolar? -1.0f: 0.0f, stereo, left, absMaxPointAllSeries);
  g.setColour(Colours::white);
  g.drawDashedLine(Line<float>(x0, upperY, x1, upperY), dashes, 2);
  g.drawDashedLine(Line<float>(x0, lowerY, x1, lowerY), dashes, 2);
}

void
FBModuleGraphDisplayComponent::PaintSeries(
  juce::Graphics& g,
  juce::Colour color,
  std::vector<float> const& points,
  bool stereo, bool left,
  int maxPointsAllSeries, float absMaxPointAllSeries)
{
  if (points.empty())
    return;

  Path path;
  path.startNewSubPath(PointLocation(points, 0, stereo, left, maxPointsAllSeries, absMaxPointAllSeries));
  for (int i = 1; i < points.size(); i++)
    path.lineTo(PointLocation(points, i, stereo, left, maxPointsAllSeries, absMaxPointAllSeries));
  g.setColour(color);
  g.strokePath(path, PathStrokeType(1.0f));
}

void
FBModuleGraphDisplayComponent::paint(Graphics& g)
{
  float absMaxPointAllSeries = 1.0f;
  bool stereo = !_data->series.primarySeries.r.empty();
  int maxPointsAllSeries = static_cast<int>(_data->series.primarySeries.l.size());
  for (int i = 0; i < _data->series.primarySeries.l.size(); i++)
  {
    absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series.primarySeries.l[i]));
    if(stereo)
      absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series.primarySeries.r[i]));
  }
  for (int i = 0; i < _data->series.secondarySeries.size(); i++)
  {
    maxPointsAllSeries = std::max(maxPointsAllSeries, static_cast<int>(_data->series.secondarySeries[i].points.l.size()));
    for (int j = 0; j < _data->series.secondarySeries[i].points.l.size(); j++)
    {
      absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series.secondarySeries[i].points.l[j]));
      if (stereo)
        absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->series.secondarySeries[i].points.r[j]));
    }
  }

  g.setColour(Colours::darkgrey);
  g.drawText(_data->series.moduleName + " " + _data->series.text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data->series.secondarySeries.size(); i++)
  {
    int marker = _data->series.secondarySeries[i].marker;
    auto const& points = _data->series.secondarySeries[i].points;
    PaintSeries(g, Colours::grey, points.l, stereo, true, maxPointsAllSeries, absMaxPointAllSeries);
    if(stereo)
      PaintSeries(g, Colours::grey, points.r, stereo, false, maxPointsAllSeries, absMaxPointAllSeries);
    if (marker != -1 && _data->drawMarkers)
    {
      assert(!stereo);
      PaintMarker(g, points.l, marker, false, true, maxPointsAllSeries, absMaxPointAllSeries);
    }
  }
  
  PaintSeries(g, Colours::white, _data->series.primarySeries.l, stereo, true, maxPointsAllSeries, absMaxPointAllSeries);
  if(stereo)
    PaintSeries(g, Colours::white, _data->series.primarySeries.r, stereo, false, maxPointsAllSeries, absMaxPointAllSeries);
  if(_data->drawMarkers)
    for (int i = 0; i < _data->series.primaryMarkers.size(); i++)
    {
      assert(!stereo);
      PaintMarker(g, _data->series.primarySeries.l, _data->series.primaryMarkers[i], false, true, maxPointsAllSeries, absMaxPointAllSeries);
    }

  if (_data->drawClipBoundaries)
  {
    PaintClipBoundaries(g, stereo, false, absMaxPointAllSeries);
    if(stereo)
      PaintClipBoundaries(g, stereo, true, absMaxPointAllSeries);
  }
}