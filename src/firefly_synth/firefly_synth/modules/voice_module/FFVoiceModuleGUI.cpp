#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleGUI.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeVoiceModuleSectionPorta(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaType, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaMode, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
  auto sync = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaSync, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto time = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaTime, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, time));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, time, Slider::SliderStyle::LinearHorizontal));
  auto bars = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaBars, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bars));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bars));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeVoiceModuleSectionPitch(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 1 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto coarse = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Coarse, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, coarse));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, coarse, Slider::SliderStyle::LinearHorizontal));
  auto fine = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Fine, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fine));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fine, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeVoiceModuleTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 1, 0 });
  grid->Add(0, 0, MakeVoiceModuleSectionPitch(plugGUI));
  grid->Add(0, 1, MakeVoiceModuleSectionPorta(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeVoiceModuleGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::VoiceModule].name;
  tabs->addTab(name, {}, MakeVoiceModuleTab(plugGUI), false);
  return tabs;
}