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
  int maxPoints, int marker, bool stereo, bool left)
{
  g.setColour(Colours::white);
  auto xy = PointLocation(points, maxPoints, marker, stereo, left);
  float x = xy.getX() - HalfMarkerSize;
  float y = xy.getY() - HalfMarkerSize;
  g.fillEllipse(x, y, MarkerSize, MarkerSize);
}

Point<float>
FBModuleGraphDisplayComponent::PointLocation(
  std::vector<float> const& points,
  int maxPoints, int point, 
  bool stereo, bool left) const
{
  float pointValue = points[point];
  if (_data->bipolar)
    pointValue = FBToUnipolar(pointValue);
  if (stereo)
    pointValue = left ? pointValue * 0.5f : 0.5f + pointValue * 0.5f;
  float y = HalfMarkerSize + (1.0f - pointValue) * (getHeight() - MarkerSize);
  float x = HalfMarkerSize + static_cast<float>(point) / maxPoints * (getWidth() - MarkerSize);
  return { x, y };
}

void
FBModuleGraphDisplayComponent::PaintSeries(
  Graphics& g, Colour color, std::vector<float> const& points, 
  int maxPoints, bool stereo, bool left)
{
  if (points.empty())
    return;

  Path path;
  path.startNewSubPath(PointLocation(points, maxPoints, 0, stereo, left));
  for (int i = 1; i < points.size(); i++)
    path.lineTo(PointLocation(points, maxPoints, i, stereo, left));
  g.setColour(color);
  g.strokePath(path, PathStrokeType(1.0f));
}

void
FBModuleGraphDisplayComponent::paint(Graphics& g)
{
  bool stereo = !_data->primarySeries.r.empty();
  int maxPoints = static_cast<int>(_data->primarySeries.l.size());
  for (int i = 0; i < _data->secondarySeries.size(); i++)
    maxPoints = std::max(maxPoints, static_cast<int>(_data->secondarySeries[i].points.l.size()));

  g.setColour(Colours::darkgrey);
  g.drawText(_data->moduleName + " " + _data->text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data->secondarySeries.size(); i++)
  {
    int marker = _data->secondarySeries[i].marker;
    auto const& points = _data->secondarySeries[i].points;
    PaintSeries(g, Colours::grey, points.l, maxPoints, stereo, true);
    if(stereo)
      PaintSeries(g, Colours::grey, points.r, maxPoints, stereo, false);
    if (marker != -1 && _data->drawMarkers)
    {
      assert(!stereo);
      PaintMarker(g, points.l, maxPoints, marker, false, true);
    }
  }
  
  PaintSeries(g, Colours::white, _data->primarySeries.l, maxPoints, stereo, true);
  if(stereo)
    PaintSeries(g, Colours::white, _data->primarySeries.r, maxPoints, stereo, false);
  if(_data->drawMarkers)
    for (int i = 0; i < _data->primaryMarkers.size(); i++)
    {
      assert(!stereo);
      PaintMarker(g, _data->primarySeries.l, maxPoints, _data->primaryMarkers[i], false, true);
    }
}