#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(
  FBRuntimeTopo const* topo, 
  IFBHostGUIContext* hostContext):
FBPlugGUI()
{
  addAndMakeVisible(FFMakeMasterGUI(topo, 0, this, hostContext));
  addAndMakeVisible(FFMakeGLFOGUI(topo, 0, this, hostContext));
  resized();
}

void 
FFPlugGUI::resized()
{
  auto local = getLocalBounds();
  auto rows = getNumChildComponents();
  for (int i = 0; i < getNumChildComponents(); i++)
  {
    Rectangle<int> bounds;
    bounds.setLeft(0);
    bounds.setWidth(local.getWidth());
    bounds.setHeight(local.getHeight() / rows);
    bounds.setTop(i * local.getHeight() / rows);
    getChildComponent(i)->setBounds(bounds);
    getChildComponent(i)->resized();
  }
}