#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleGUI.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeVoiceModuleSectionMain(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 0, 0, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto bend = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBend, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bend));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bend, Slider::SliderStyle::RotaryVerticalDrag));
  auto bendRange = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBendRange, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bendRange));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bendRange));
  auto bendTarget = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBendTarget, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bendTarget));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bendTarget));
  auto mod = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::ModWheel, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mod));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mod, Slider::SliderStyle::RotaryVerticalDrag));
  auto tuning = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::TuningMode, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuning));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tuning));
  auto smooth = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::HostSmoothTime, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smooth));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smooth, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeVoiceModuleSectionPorta(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 0, 0, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto bend = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBend, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bend));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bend, Slider::SliderStyle::RotaryVerticalDrag));
  auto bendRange = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBendRange, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bendRange));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bendRange));
  auto bendTarget = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::PitchBendTarget, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bendTarget));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bendTarget));
  auto mod = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::ModWheel, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mod));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mod, Slider::SliderStyle::RotaryVerticalDrag));
  auto tuning = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::TuningMode, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, tuning));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, tuning));
  auto smooth = topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)FFMasterParam::HostSmoothTime, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smooth));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smooth, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeVoiceModuleTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  grid->Add(0, 0, MakeVoiceModuleSectionMain(plugGUI));
  grid->Add(0, 1, MakeVoiceModuleSectionPorta(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeVoiceModuleGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::Master].name;
  tabs->addTab(name, {}, MakeMasterTab(plugGUI), false);
  return tabs;
}