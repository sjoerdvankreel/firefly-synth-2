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
  // TODO
  Path p;
  g.fillAll(Colours::black);
  p.startNewSubPath(0.0, 0.0);
  for (int i = 0; i < getWidth() && i < _data.points.size(); i++)
    p.lineTo((float)i, _data.points[i] * getHeight());
  g.setColour(Colours::white);
  g.strokePath(p, PathStrokeType(1.0f));
}

void
FBModuleGraphComponent::RequestRerender(int moduleIndex, int moduleSlot)
{
  // TODO
  auto const& topo = _plugGUI->Topo()->static_.modules[moduleIndex];
  if (topo.renderGraph == nullptr)
    return;
  _data.text.clear();
  _data.points.clear();
  topo.renderGraph(_plugGUI, moduleSlot, &_data);
  repaint();
}