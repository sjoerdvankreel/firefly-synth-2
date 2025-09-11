#include <firefly_synth/modules/mod_matrix/FFModMatrixGraph.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

using namespace juce;

FFModMatrixGraph::
FFModMatrixGraph(FFPlugGUI* plugGUI, FFModMatrixGraphType type) :
_plugGUI(plugGUI), _type(type) {}

void
FFModMatrixGraph::paint(Graphics& g)
{
  auto bounds = getBounds().toFloat();
  std::vector<float> yNormalized = {};
  switch (_type)
  {
  case FFModMatrixGraphType::Source:
    for (int i = 0; i < bounds.getWidth(); i++)
      yNormalized.push_back(FBToUnipolar(std::sin(2.0f * FBPi * i / (float)bounds.getWidth())));
    break;
  default:
    break;
  }

  Path path;
  path.startNewSubPath(bounds.getX(), bounds.getY() + bounds.getHeight() * yNormalized[0]);
  for (int i = 1; i < yNormalized.size(); i++)
    path.lineTo(bounds.getX() + 1.0f, bounds.getY() + bounds.getHeight() * yNormalized[i]);
  g.setColour(Colours::white);
  g.strokePath(path, PathStrokeType(1.0f));  
}