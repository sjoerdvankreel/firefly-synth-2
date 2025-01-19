#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(
  FBRuntimeTopo const* topo, 
  IFBHostGUIContext* hostContext):
FBPlugGUI()
{
  addAndMakeVisible(FFMakeMasterGUI(topo, 0, this, hostContext));
  addAndMakeVisible(FFMakeGLFOGUI(topo, 0, this, hostContext));
  addAndMakeVisible(FFMakeGFilterGUI(topo, 0, this, hostContext));
  addAndMakeVisible(FFMakeGFilterGUI(topo, 1, this, hostContext));
  addAndMakeVisible(FFMakeOsciGUI(topo, 0, this, hostContext));
  addAndMakeVisible(FFMakeOsciGUI(topo, 1, this, hostContext));
  resized();
}

void 
FFPlugGUI::resized()
{
  auto local = getLocalBounds();
  auto rows = getNumChildComponents();
  for (int i = 0; i < getNumChildComponents(); i++)
  {
    int x = local.getX();
    int w = local.getWidth();
    int h = local.getHeight() / rows;
    int y = local.getY() + i * local.getHeight() / rows;
    getChildComponent(i)->setBounds(x, y, w, h);
    getChildComponent(i)->resized();
  }
}