#include <firefly_synth/gui/FFPatchGUI.hpp>
#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

Component*
FFMakePatchGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto context = plugGUI->HostContext();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Patch:"));
  
  auto name = plugGUI->StoreComponent<FBAutoSizeLabel2>(150);
  name->setText(context->PatchName(), dontSendNotification);
  grid->Add(0, 1, name);

  auto load = plugGUI->StoreComponent<FBAutoSizeButton>("Load");
  load->setTooltip("Load From File");
  load->onClick = [plugGUI]() { plugGUI->LoadPatchFromFile(); };
  grid->Add(0, 2, load);

  auto reload = plugGUI->StoreComponent<FBAutoSizeButton>("Reload");
  reload->setTooltip("Reload " + context->PatchName());
  reload->onClick = [plugGUI]() { plugGUI->ReloadPatch(); };
  reload->setEnabled(false);
  grid->Add(0, 3, reload);

  plugGUI->onPatchLoaded = [reload]() { reload->setEnabled(true); };
  plugGUI->onPatchNameChanged = [reload, name](auto const& newName) {
    name->setText(newName, dontSendNotification); 
    reload->setTooltip("Reload " + newName);
  };

  auto session = plugGUI->StoreComponent<FBAutoSizeButton>("Session");
  session->setTooltip("Reload From Session");
  session->onClick = [plugGUI]() { plugGUI->ReloadSession(); };
  grid->Add(0, 4, session);

  auto save = plugGUI->StoreComponent<FBAutoSizeButton>("Save");
  save->setTooltip("Save To File");
  save->onClick = [plugGUI]() { plugGUI->SavePatchToFile(); };
  grid->Add(0, 5, save);

  auto init = plugGUI->StoreComponent<FBAutoSizeButton>("Init");
  init->setTooltip("Init Defaults");
  init->onClick = [plugGUI]() { plugGUI->InitPatch(); };
  grid->Add(0, 6, init);

  auto preset = plugGUI->StoreComponent<FBAutoSizeButton>("Preset");
  preset->setTooltip("Load Preset");
  preset->onClick = [plugGUI, preset]() { plugGUI->LoadPreset(preset); };
  grid->Add(0, 7, preset);

  grid->Add(0, 8, plugGUI->StoreComponent<FBFillerComponent>(13, 1));
  grid->MarkSection({ { 0, 0 }, { 1, 9 } });

  auto section = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(section);
}