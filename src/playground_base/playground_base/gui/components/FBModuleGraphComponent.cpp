#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

FBModuleGraphComponent::
FBModuleGraphComponent(FBGraphProcState* state):
Component()
{
  _data.state = state;
}

void
FBModuleGraphComponent::RequestRerender(int paramIndex)
{
  if (GetRendererForParam(paramIndex) == nullptr)
    return;
  _tweakedParamByUI = paramIndex;
  repaint();
}

FBModuleGraphRenderer 
FBModuleGraphComponent::GetRendererForParam(int index)
{
  auto const* runtimeTopo = _data.state->moduleState.topo;
  int staticModuleIndex = runtimeTopo->params[index].topoIndices.module.index;
  auto const& staticModule = runtimeTopo->static_.modules[staticModuleIndex];
  return staticModule.renderGraph;
}

void
FBModuleGraphComponent::paint(Graphics& g)
{
  if (_tweakedParamByUI == -1)
    return;
  auto renderer = GetRendererForParam(_tweakedParamByUI);
  if (renderer == nullptr)
    return;

  _data.text.clear();
  _data.points.clear();
  renderer(&_data);

  // TODO
  Path p;
  g.fillAll(Colours::blue);
  p.startNewSubPath(0.0, 0.0);
  for (int i = 0; i < getWidth() && i < _data.points.size(); i++)
    p.lineTo((float)i, _data.points[i] * getHeight());
  g.setColour(Colours::white);
  g.strokePath(p, PathStrokeType(1.0f));
}