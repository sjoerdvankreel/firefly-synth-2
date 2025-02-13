#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>
#include <playground_base/gui/components/FBModuleGraphDisplayComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

FBModuleGraphDisplayComponent::
FBModuleGraphDisplayComponent(FBModuleGraphComponentData const* data):
Component(),
_data(data) {}

juce::Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  std::vector<float> const& points, 
  int maxPoints, int point) const
{
  float x = (float)point / maxPoints * getWidth();
  float y = (1.0f - points[point]) * getHeight();
  return { x, y };
}

void 
FBModuleGraphDisplayComponent::PaintMarker(
  Graphics& g, std::vector<float> const& points, 
  int maxPoints, int marker)
{
  g.setColour(Colours::white);
  auto markerXY = PointLocation(points, maxPoints, marker);
  g.fillEllipse(markerXY.getX() - 4.0f, markerXY.getY() - 4.0f, 8.0f, 8.0f);
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

  g.fillAll(Colours::black);
  g.setColour(Colours::darkgrey);
  g.drawText(_data->moduleName + " " + _data->text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data->secondarySeries.size(); i++)
  {
    int marker = _data->secondarySeries[i].marker;
    auto const& points = _data->secondarySeries[i].points;
    PaintSeries(g, Colours::grey, points, maxPoints);
    if (marker != -1)
      PaintMarker(g, points, maxPoints, marker);
  }
  
  PaintSeries(g, Colours::white, _data->primarySeries, maxPoints);
  for (int i = 0; i < _data->primaryMarkers.size(); i++)
    PaintMarker(g, _data->primarySeries, maxPoints, _data->primaryMarkers[i]);
}