#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>

#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(FBRuntimeTopo const* topo, FBHostGUIContext* hostContext):
FBPlugGUI(topo, hostContext)
{
  SetupGUI();
  InitAllDependencies();
  resized();
}

void
FFPlugGUI::resized()
{
  getChildComponent(0)->setBounds(getLocalBounds());
  getChildComponent(0)->resized();
}

void 
FFPlugGUI::SetupGUI()
{
  auto grid = StoreComponent<FBGridComponent>(5, 2);
  grid->Add(0, 0, 1, 1, FFMakeMasterGUI(this));
  grid->Add(0, 1, 1, 1, StoreComponent<FBModuleGraphComponent>());
  grid->Add(1, 0, 1, 2, FFMakeGLFOGUI(this));
  grid->Add(2, 0, 1, 2, FFMakeGFilterGUI(this));
  grid->Add(3, 0, 1, 2, FFMakeOsciGUI(this));
  grid->Add(4, 0, 1, 2, FFMakeEnvGUI(this));
  addAndMakeVisible(grid);
}