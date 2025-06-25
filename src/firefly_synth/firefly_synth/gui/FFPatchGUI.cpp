#include <firefly_synth/gui/FFPatchGUI.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>

using namespace juce;

static Component*
MakePatchTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeButton>("Load"));
  grid->Add(0, 1, plugGUI->StoreComponent<FBAutoSizeButton>("Save"));
  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeButton>("Clear"));
  grid->MarkSection({ { 0, 0 }, { 1, 3 } });
  return grid;
}

Component*
FFMakePatchGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  tabs->addTab("Patch", {}, MakePatchTab(plugGUI), false);
  return tabs;
}