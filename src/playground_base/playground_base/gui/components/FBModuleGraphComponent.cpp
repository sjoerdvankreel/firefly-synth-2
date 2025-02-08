#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

FBModuleGraphComponent::
FBModuleGraphComponent(FBGraphRenderState* renderState):
Component()
{
  _data.renderState = renderState;
}

void
FBModuleGraphComponent::RequestRerender(int moduleIndex)
{
  if (!PrepareForRender(moduleIndex))
    return;
  _tweakedModuleByUI = moduleIndex;
  repaint();
}

juce::Point<float>
FBModuleGraphComponent::PointLocation(
  std::vector<float> const& points, 
  int maxPoints, int point) const
{
  float x = (float)point / maxPoints * getWidth();
  float y = (1.0f - points[point]) * getHeight();
  return { x, y };
}

void 
FBModuleGraphComponent::PaintMarker(
  Graphics& g, std::vector<float> const& points, 
  int maxPoints, int marker)
{
  g.setColour(Colours::white);
  auto markerXY = PointLocation(points, maxPoints, marker);
  g.fillEllipse(markerXY.getX() - 4.0f, markerXY.getY() - 4.0f, 8.0f, 8.0f);
}

void
FBModuleGraphComponent::PaintSeries(
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

bool 
FBModuleGraphComponent::PrepareForRender(int moduleIndex)
{
  if (moduleIndex == -1)
    return false;
  auto& moduleState = _data.renderState->ModuleState();
  auto const& topoIndices = moduleState.topo->modules[moduleIndex].topoIndices;
  auto const& staticModule = moduleState.topo->static_.modules[topoIndices.index];
  if (staticModule.renderGraph == nullptr)
    return false;
  _renderer = staticModule.renderGraph;
  moduleState.moduleSlot = topoIndices.slot;
  return true;
}

void
FBModuleGraphComponent::paint(Graphics& g)
{
  if (_tweakedModuleByUI == -1)
    return;
  if (!PrepareForRender(_tweakedModuleByUI))
    return;

  _data.text.clear();
  _data.primarySeries.clear();
  _data.primaryMarkers.clear();
  _data.secondarySeries.clear();
  _renderer(&_data);

  int maxPoints = _data.primarySeries.size();
  for (int i = 0; i < _data.secondarySeries.size(); i++)
    maxPoints = std::max(maxPoints, (int)_data.secondarySeries[i].points.size());

  g.fillAll(Colours::black);
  g.setColour(Colours::darkgrey);
  auto const* runtimeTopo = _data.renderState->ModuleState().topo;
  std::string moduleName = runtimeTopo->modules[_tweakedModuleByUI].name;
  g.drawText(moduleName + " " + _data.text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data.secondarySeries.size(); i++)
  {
    int marker = _data.secondarySeries[i].marker;
    auto const& points = _data.secondarySeries[i].points;
    PaintSeries(g, Colours::grey, points, maxPoints);
    if (marker != -1)
      PaintMarker(g, points, maxPoints, marker);
  }
  
  PaintSeries(g, Colours::white, _data.primarySeries, maxPoints);
  for (int i = 0; i < _data.primaryMarkers.size(); i++)
    PaintMarker(g, _data.primarySeries, maxPoints, _data.primaryMarkers[i]);
}