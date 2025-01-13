#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/topo/FBStaticGUITopo.hpp>

using namespace juce;

FBPlugGUI::
FBPlugGUI(FBStaticGUITopo const* topo):
FBPlugGUIContext(topo)
{
  setOpaque(true);
  setVisible(true);
  setSize(topo->plugWidth, GetHeightForAspectRatio(topo->plugWidth));
}

void 
FBPlugGUI::paint(Graphics& g)
{
  g.fillAll(Colours::black);
}

void
FBPlugGUI::RemoveFromDesktop()
{
  removeFromDesktop();
}

void
FBPlugGUI::AddToDesktop(void* parent)
{
  addToDesktop(0, parent);
}

void
FBPlugGUI::SetVisible(bool visible)
{
  setVisible(visible);
}

void
FBPlugGUI::RequestRescale(float combinedScale)
{
  setTransform(AffineTransform::scale(combinedScale));
}

void
FBPlugGUI::SetParamNormalized(int index, float normalized)
{
  GetParamControlForIndex(index)->SetValueNormalized(normalized);
}