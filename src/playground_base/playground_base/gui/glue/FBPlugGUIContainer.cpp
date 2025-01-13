#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBPlugGUIContainer.hpp>
#include <playground_base/gui/components/FBParamControl.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

using namespace juce;

FBPlugGUIContainer::
~FBPlugGUIContainer() {}

FBPlugGUIContainer::
FBPlugGUIContainer(
  FBRuntimeTopo const* topo,
  IFBHostGUIContext* context):
FBPlugGUIContext(&topo->static_.gui),
_gui(topo->static_.gui.factory(topo, context))
{
  setOpaque(true);
  setVisible(true);
  int plugWidth = topo->static_.gui.plugWidth;
  setSize(plugWidth, GetHeightForAspectRatio(plugWidth));
}

void 
FBPlugGUIContainer::paint(Graphics& g)
{
  g.fillAll(Colours::black);
}

void
FBPlugGUIContainer::RemoveFromDesktop()
{
  removeFromDesktop();
}

void
FBPlugGUIContainer::AddToDesktop(void* parent)
{
  addToDesktop(0, parent);
}

void
FBPlugGUIContainer::SetVisible(bool visible)
{
  setVisible(visible);
}

void
FBPlugGUIContainer::RequestRescale(float combinedScale)
{
  _gui->setTransform(AffineTransform::scale(combinedScale));
}

void
FBPlugGUIContainer::SetParamNormalized(int index, float normalized)
{
  _gui->GetParamControlForIndex(index)->SetValueNormalized(normalized);
}