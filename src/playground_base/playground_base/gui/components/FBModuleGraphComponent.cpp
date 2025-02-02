#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

FBModuleGraphComponent::
FBModuleGraphComponent(FBGraphRenderState* state):
Component()
{
  _data.state = state;
}

juce::Point<float>
FBModuleGraphComponent::PointLocation(
  std::vector<float> const& points, int point) const
{
  float x = (float)point / points.size() * getWidth();
  float y = (1.0f - points[point]) * getHeight();
  return { x, y };
}

void
FBModuleGraphComponent::RequestRerender(int paramIndex)
{
  if (!PrepareForRender(paramIndex))
    return;
  _tweakedParamByUI = paramIndex;
  repaint();
}

void
FBModuleGraphComponent::PaintSeries(
  Graphics& g, Colour color, std::vector<float> const& points)
{
  if (points.empty())
    return;

  Path path;
  path.startNewSubPath(PointLocation(points, 0));
  for (int i = 1; i < points.size(); i++)
    path.lineTo(PointLocation(points, i));
  g.setColour(color);
  g.strokePath(path, PathStrokeType(1.0f));
}

bool 
FBModuleGraphComponent::PrepareForRender(int paramIndex)
{
  auto& moduleState = _data.state->ModuleState();
  auto const& topoIndices = moduleState.topo->params[paramIndex].topoIndices.module;
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
  if (_tweakedParamByUI == -1)
    return;
  if (!PrepareForRender(_tweakedParamByUI))
    return;

  _data.text.clear();
  _data.primaryPoints.clear();
  _data.secondaryData.clear();
  _renderer(&_data);

  g.fillAll(Colours::black);
  g.setColour(Colours::darkgrey);
  auto const* runtimeTopo = _data.state->ModuleState().topo;
  std::string moduleName = runtimeTopo->ModuleAtParamIndex(_tweakedParamByUI)->name;
  g.drawText(moduleName + " " + _data.text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data.secondaryData.size(); i++)
  {
    auto const& points = _data.secondaryData[i].points;
    PaintSeries(g, Colours::grey, points);
    int marker = _data.secondaryData[i].marker;
    if (marker != -1)
    {
      g.setColour(Colours::white);
      auto markerXY = PointLocation(_data.secondaryData[i].points, marker);
      g.fillEllipse(markerXY.getX() - 4.0f, markerXY.getY() - 4.0f, 8.0f, 8.0f); // TODO
    }
  }
  PaintSeries(g, Colours::white, _data.primaryPoints);
}