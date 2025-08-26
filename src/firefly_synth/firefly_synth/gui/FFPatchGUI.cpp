#include <firefly_synth/gui/FFPatchGUI.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

Component*
FFMakePatchGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Patch"));
  auto load = plugGUI->StoreComponent<FBAutoSizeButton>("Load");
  load->onClick = [plugGUI]() { plugGUI->LoadPatchFromFile(); };
  grid->Add(0, 1, load);
  auto save = plugGUI->StoreComponent<FBAutoSizeButton>("Save");
  save->onClick = [plugGUI]() { plugGUI->SavePatchToFile(); };
  grid->Add(0, 2, save);
  auto init = plugGUI->StoreComponent<FBAutoSizeButton>("Init");
  init->onClick = [plugGUI]() { plugGUI->InitPatch(); };
  grid->Add(0, 3, init);
  auto factory = plugGUI->StoreComponent<FBAutoSizeButton>("Factory");
  factory->setEnabled(false);
  grid->Add(0, 4, factory);
  grid->MarkSection({ { 0, 0 }, { 1, 5 } });
  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
}