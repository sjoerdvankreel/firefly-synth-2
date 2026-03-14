#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/other/FFOtherGUI.hpp>
#include <firefly_synth/modules/other/FFOtherTopo.hpp>
#include <firefly_synth/modules/settings/FFSettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBParamDisplay.hpp>
#include <firefly_base/gui/controls/FBInstanceNameEditor.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSettingsGUI(FFPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0 }, std::vector<int> { 1 });
  grid->Add(0, 0, FFMakeGUISettingsGUI(plugGUI));
  grid->Add(1, 0, FFMakeSettingsGUIMain(plugGUI));
  grid->Add(2, 0, FFMakeSettingsGUITuning(plugGUI));
  return grid;
}

Component*
FFMakeOtherGUI(FFPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0 } );
  auto instanceNameEditor = plugGUI->StoreComponent<FBInstanceNameEditor>(plugGUI, 100);
  plugGUI->onInstanceNameChanged = [instanceNameEditor](auto const& name) { instanceNameEditor->setText(name, false); };
  grid->Add(0, 0, instanceNameEditor);
  auto settingsButton = plugGUI->StoreComponent<FBAutoSizeButton>(plugGUI, "Settings");
  auto settingsGUI = MakeSettingsGUI(plugGUI);
  settingsButton->setTooltip("GUI/Engine/Microtuning Settings");
  settingsButton->onClick = [plugGUI, settingsGUI]() {
    dynamic_cast<FFPlugGUI&>(*plugGUI).ShowOverlayComponent("Settings", (int)FFModuleType::Other, 0, settingsGUI, 400, 275, false, []() {});
  };

  grid->Add(0, 1, settingsButton);
  auto panicButton = plugGUI->StoreComponent<FBAutoSizeButton>(plugGUI, "Panic");
  panicButton->setTooltip("Reset Voices And Delay Lines"); 
  panicButton->onClick = [plugGUI]() { dynamic_cast<FFPlugGUI&>(*plugGUI).FlushAudio(); };
  grid->Add(0, 2, panicButton);
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  auto margin = plugGUI->StoreComponent<FBMarginComponent>(plugGUI, false, true, false, false, card);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::Other, 0, margin);
}