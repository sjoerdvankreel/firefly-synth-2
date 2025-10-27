#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/settings/FFSettingsGUI.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniGUI.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSettingsTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0 });
  auto receiveNotes = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::ReceiveNotes, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, receiveNotes));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, receiveNotes));
  auto smooth = topo->audio.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsParam::HostSmoothTime, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smooth));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smooth, Slider::SliderStyle::RotaryVerticalDrag));
  grid->Add(0, 2, plugGUI->StoreComponent<FBFillerComponent>(8, 1));
  grid->Add(1, 2, plugGUI->StoreComponent<FBFillerComponent>(8, 1));
  grid->MarkSection({ { 0, 0 }, { 2, 3 } });
  auto subSection = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(subSection);
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