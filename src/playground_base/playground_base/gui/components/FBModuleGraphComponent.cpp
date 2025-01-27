#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

void
FBModuleGraphComponent::paint(Graphics& g)
{
  // TODO
  Path p;
  g.fillAll(Colours::black);
  p.startNewSubPath(0.0, 0.0);
  for (int i = 0; i < getWidth(); i++)
  {
    float phase = (float)i / (getWidth() + 1);
    float sine = (std::sin(phase * 2.0f * std::numbers::pi_v<float>) + 1.0f) * 0.5f;
    p.lineTo((float)i, sine * getHeight());
  }      
  g.setColour(Colours::white);
  g.strokePath(p, PathStrokeType(1.0f));
}