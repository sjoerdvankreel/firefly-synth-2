#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

FBModuleGraphComponent::
FBModuleGraphComponent(FBPlugGUI const* plugGUI):
Component(),
_plugGUI(plugGUI) {}

void
FBModuleGraphComponent::paint(Graphics& g)
{
  if (_tweakedParamByUI == -1)
    return;

  _data.text.clear();
  _data.points.clear();
  auto const& topo = _plugGUI->Topo()->static_.modules[_plugGUI->Topo()->params[_tweakedParamByUI].topoIndices.module.index];
  if (topo.renderGraph == nullptr)
    return;

  topo.renderGraph(_plugGUI, _plugGUI->Topo()->params[_tweakedParamByUI].topoIndices.module.slot, &_data);

  // TODO
  Path p;
  g.fillAll(Colours::blue);
  p.startNewSubPath(0.0, 0.0);
  for (int i = 0; i < getWidth() && i < _data.points.size(); i++)
    p.lineTo((float)i, _data.points[i] * getHeight());
  g.setColour(Colours::white);
  g.strokePath(p, PathStrokeType(1.0f));
}

void
FBModuleGraphComponent::RequestRerender(int paramIndex)
{
  // TODO
  auto const& topo = _plugGUI->Topo()->static_.modules[_plugGUI->Topo()->params[paramIndex].topoIndices.module.index];
  if (topo.renderGraph == nullptr)
    return;
  _tweakedParamByUI = paramIndex;
  repaint();
}