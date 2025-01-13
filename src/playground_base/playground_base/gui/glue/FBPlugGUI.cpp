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
FBPlugGUIBase::paint(Graphics& g)
{
  g.fillAll(Colours::black);
}

void
FBPlugGUIBase::RemoveFromDesktop()
{
  removeFromDesktop();
}

void
FBPlugGUIBase::AddToDesktop(void* parent)
{
  addToDesktop(0, parent);
}

void
FBPlugGUIBase::SetVisible(bool visible)
{
  setVisible(visible);
}

void
FBPlugGUIBase::SetParamNormalized(int index, float normalized)
{
  GetParamControlForIndex(index)->SetValueNormalized(normalized);
}