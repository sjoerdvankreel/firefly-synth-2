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

Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  std::vector<float> const& points,
  int point, bool stereo, bool left,
  int maxPointsAllSeries, float absMaxPointAllSeries) const
{
  float pointValue = points[point];
  if (_data->bipolar)
    pointValue = FBToUnipolar(pointValue);
  if (stereo)
    pointValue = left ? pointValue * 0.5f : 0.5f + pointValue * 0.5f;
  float y = HalfMarkerSize + (1.0f - (pointValue / absMaxPointAllSeries)) * (getHeight() - MarkerSize);
  float x = HalfMarkerSize + static_cast<float>(point) / maxPointsAllSeries * (getWidth() - MarkerSize);
  return { x, y };
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
  bool stereo = !_data->primarySeries.r.empty();
  int maxPointsAllSeries = static_cast<int>(_data->primarySeries.l.size());
  for (int i = 0; i < _data->primarySeries.l.size(); i++)
  {
    absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->primarySeries.l[i]));
    absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->primarySeries.r[i]));
  }
  for (int i = 0; i < _data->secondarySeries.size(); i++)
  {
    maxPointsAllSeries = std::max(maxPointsAllSeries, static_cast<int>(_data->secondarySeries[i].points.l.size()));
    for (int j = 0; j < _data->secondarySeries[i].points.l.size(); j++)
    {
      absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->secondarySeries[i].points.l[j]));
      absMaxPointAllSeries = std::max(absMaxPointAllSeries, std::abs(_data->secondarySeries[i].points.r[j]));
    }
  }

  g.setColour(Colours::darkgrey);
  g.drawText(_data->moduleName + " " + _data->text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data->secondarySeries.size(); i++)
  {
    int marker = _data->secondarySeries[i].marker;
    auto const& points = _data->secondarySeries[i].points;
    PaintSeries(g, Colours::grey, points.l, stereo, true, maxPointsAllSeries, absMaxPointAllSeries);
    if(stereo)
      PaintSeries(g, Colours::grey, points.r, stereo, false, maxPointsAllSeries, absMaxPointAllSeries);
    if (marker != -1 && _data->drawMarkers)
    {
      assert(!stereo);
      PaintMarker(g, points.l, marker, false, true, maxPointsAllSeries, absMaxPointAllSeries);
    }
  }
  
  PaintSeries(g, Colours::white, _data->primarySeries.l, stereo, true, maxPointsAllSeries, absMaxPointAllSeries);
  if(stereo)
    PaintSeries(g, Colours::white, _data->primarySeries.r, stereo, false, maxPointsAllSeries, absMaxPointAllSeries);
  if(_data->drawMarkers)
    for (int i = 0; i < _data->primaryMarkers.size(); i++)
    {
      assert(!stereo);
      PaintMarker(g, _data->primarySeries.l, _data->primaryMarkers[i], false, true, maxPointsAllSeries, absMaxPointAllSeries);
    }
}