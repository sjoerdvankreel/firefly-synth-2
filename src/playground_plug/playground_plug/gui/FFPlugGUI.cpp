#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>

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
  getChildComponent(0)->resized();
}

void 
FFPlugGUI::SetupGUI()
{
  auto grid = AddComponent<FBGridComponent>(5, 1);
  grid->Add(0, 0, FFMakeMasterGUI(this));
  grid->Add(1, 0, FFMakeGLFOGUI(this));
  grid->Add(2, 0, FFMakeGFilterGUI(this));
  grid->Add(3, 0, FFMakeOsciGUI(this));
  grid->Add(4, 0, FFMakeEnvGUI(this));
}