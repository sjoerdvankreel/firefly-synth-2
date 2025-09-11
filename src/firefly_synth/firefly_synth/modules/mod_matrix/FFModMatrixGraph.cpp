#include <firefly_synth/modules/mod_matrix/FFModMatrixGraph.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/gui/shared/FBGUI.hpp>

using namespace juce;

FFModMatrixGraph::
FFModMatrixGraph(FFPlugGUI* plugGUI, FFModMatrixGraphType type) :
_plugGUI(plugGUI), _type(type) {}

void
FFModMatrixGraph::paint(Graphics& g)
{
  std::string text = {};
  std::vector<float> yNormalized = {};
  auto bounds = getBounds().toFloat().reduced(2.0f);
  switch (_type)
  {
  case FFModMatrixGraphType::Source:
    text = "Source";
    for (int i = 0; i < bounds.getWidth(); i++)
      yNormalized.push_back(FBToUnipolar(std::sin(2.0f * FBPi * i / (float)bounds.getWidth())));
    break;
  default:
    yNormalized.push_back(0.5);
    yNormalized.push_back(0.5);
    break;
  }

  g.setColour(Colours::darkgrey);
  g.setFont(FBGUIGetFont().withHeight(16.0f));
  g.drawText(text, bounds, Justification::centred, false);

  Path path;
  path.startNewSubPath(bounds.getX(), bounds.getY() + bounds.getHeight() * (1.0f - yNormalized[0]));
  for (int i = 1; i < yNormalized.size(); i++)
    path.lineTo(bounds.getX() + i, bounds.getY() + bounds.getHeight() * (1.0f - yNormalized[i]));
  g.setColour(Colours::white);
  g.strokePath(path, PathStrokeType(1.0f));  
}