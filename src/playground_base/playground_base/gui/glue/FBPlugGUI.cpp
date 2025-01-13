#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/topo/FBStaticGUITopo.hpp>

using namespace juce;

FBPlugGUIBase::
FBPlugGUIBase(FBStaticGUITopo const* topo):
FBPlugGUIContext(topo)
{
  setOpaque(true);
  setVisible(true);
  setSize(topo->plugWidth, topo->plugWidth * topo->aspectRatioHeight / topo->aspectRatioWidth);
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