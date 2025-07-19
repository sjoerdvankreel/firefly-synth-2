#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/gui/FFPlugMatrixGUI.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

static Component*
MakePlugMatrixTab(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  auto voice = plugGUI->StoreComponent<FBAutoSizeButton>("Voice");
  voice->onClick = [plugGUI]() { plugGUI->ShowVoiceMatrix(); };
  grid->Add(0, 0, voice);
  auto global = plugGUI->StoreComponent<FBAutoSizeButton>("Global");
  global->onClick = [plugGUI]() { plugGUI->ShowGlobalMatrix(); };
  grid->Add(0, 1, global);
  grid->MarkSection({ { 0, 0 }, { 1, 2 } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakePlugMatrixGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  tabs->addTab("Matrix", {}, MakePlugMatrixTab(plugGUI), false);
  return tabs;
}