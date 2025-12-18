#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/settings/FFSettingsGUI.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSettingsSectionMain(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0 });
  auto receiveNotes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::ReceiveNotes, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, receiveNotes));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, receiveNotes));
  auto smooth = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::HostSmoothTime, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smooth));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smooth, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } }, true);
  return grid;
}

static Component*
MakeSettingsSectionTuning(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 1 });
  auto tuning = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::Tuning, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuning));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tuning));
  auto tuneOnNote = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::TuneOnNote, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuneOnNote));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tuneOnNote));
  auto tuneMasterPB = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::TuneMasterPB, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuneMasterPB));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tuneMasterPB));
  auto tuneVoiceCoarse = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::TuneVoiceCoarse, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuneVoiceCoarse));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tuneVoiceCoarse));
  auto tuneMasterMatrix = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::TuneMasterMatrix, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuneMasterMatrix));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tuneMasterMatrix));
  auto tuneVoiceMatrix = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::TuneVoiceMatrix, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuneVoiceMatrix));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tuneVoiceMatrix));
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return grid;
}

static Component*
MakeSettingsTab(FBPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  grid->Add(0, 0, MakeSettingsSectionMain(plugGUI));
  grid->Add(0, 1, MakeSettingsSectionTuning(plugGUI));
  auto section = plugGUI->StoreComponent<FBSectionComponent>(grid);
  return plugGUI->StoreComponent<FBModuleComponent>((int)FFModuleType::Settings, 0, section);
}

Component*
FFMakeSettingsGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::Settings].name;
  tabs->addTab(name, {}, MakeSettingsTab(plugGUI), false);
  return tabs;
}