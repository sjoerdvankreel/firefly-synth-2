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
  Graphics& g, std::vector<float> const& points,
  int maxPoints, int marker)
{
  g.setColour(Colours::white);
  auto xy = PointLocation(points, maxPoints, marker);
  float x = xy.getX() - HalfMarkerSize;
  float y = xy.getY() - HalfMarkerSize;
  g.fillEllipse(x, y, MarkerSize, MarkerSize);
}

Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  std::vector<float> const& points,
  int maxPoints, int point) const
{
  float pointValue = points[point];
  if (_data->bipolar)
    pointValue = FBToUnipolar(pointValue);
  float y = HalfMarkerSize + (1.0f - pointValue) * (getHeight() - MarkerSize);
  float x = HalfMarkerSize + (float)point / maxPoints * (getWidth() - MarkerSize);
  return { x, y };
}

void
FBModuleGraphDisplayComponent::PaintSeries(
  Graphics& g, Colour color, 
  std::vector<float> const& points, int maxPoints)
{
  if (points.empty())
    return;

  Path path;
  path.startNewSubPath(PointLocation(points, maxPoints, 0));
  for (int i = 1; i < points.size(); i++)
    path.lineTo(PointLocation(points, maxPoints, i));
  g.setColour(color);
  g.strokePath(path, PathStrokeType(1.0f));
}

void
FBModuleGraphDisplayComponent::paint(Graphics& g)
{
  int maxPoints = (int)_data->primarySeries.size();
  for (int i = 0; i < _data->secondarySeries.size(); i++)
    maxPoints = std::max(maxPoints, (int)_data->secondarySeries[i].points.size());

  g.setColour(Colours::darkgrey);
  g.drawText(_data->moduleName + " " + _data->text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data->secondarySeries.size(); i++)
  {
    int marker = _data->secondarySeries[i].marker;
    auto const& points = _data->secondarySeries[i].points;
    PaintSeries(g, Colours::grey, points, maxPoints);
    if (marker != -1 && _data->drawMarkers)
      PaintMarker(g, points, maxPoints, marker);
  }
  
  PaintSeries(g, Colours::white, _data->primarySeries, maxPoints);
  if(_data->drawMarkers)
    for (int i = 0; i < _data->primaryMarkers.size(); i++)
      PaintMarker(g, _data->primarySeries, maxPoints, _data->primaryMarkers[i]);
}