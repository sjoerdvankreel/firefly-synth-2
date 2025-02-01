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

void
FBModuleGraphComponent::RequestRerender(int paramIndex)
{
  if (!PrepareForRender(paramIndex))
    return;
  _tweakedParamByUI = paramIndex;
  repaint();
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
FBModuleGraphComponent::PaintSeries(
  Graphics& g, Colour color, std::vector<float> const& points)
{
  if (points.empty())
    return;

  Path p;
  float y0 = (1.0f - points[0]) * getHeight();
  p.startNewSubPath(0.0, y0);
  for (int i = 1; i < points.size(); i++)
  {
    float x = (float)i / points.size() * getWidth();
    float y = (1.0f - points[i]) * getHeight();
    p.lineTo(x, y);
  }
  g.setColour(color);
  g.strokePath(p, PathStrokeType(1.0f));
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
    g.setColour(Colours::grey);
    PaintSeries(g, Colours::grey, _data.secondaryData[i].points);
    int marker = _data.secondaryData[i].marker;
    if (marker != -1)
    {
      float yPosNorm = _data.secondaryData[i].points[marker];
      float xPosNorm = marker / (float)_data.secondaryData[i].points.size();
      float xPos = xPosNorm * getWidth();
      float yPos = (1.0f - yPosNorm) * getHeight();
      g.fillEllipse(xPos - 2.0f, yPos - 2.0f, 4.0f, 4.0f); // TODO
    }
  }
  PaintSeries(g, Colours::white, _data.primaryPoints);
}