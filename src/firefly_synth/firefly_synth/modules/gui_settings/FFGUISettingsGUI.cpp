#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto hilightMod = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightMod, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, hilightMod));
  grid->Add(0, 1, plugGUI->StoreComponent<FBGUIParamToggleButton>(plugGUI, hilightMod));
  auto hilightTweak = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightTweak, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, hilightTweak));
  grid->Add(0, 3, plugGUI->StoreComponent<FBGUIParamToggleButton>(plugGUI, hilightTweak));
  auto knobVisualsMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::KnobVisualsMode, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, knobVisualsMode));
  grid->Add(0, 5, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, knobVisualsMode));
  auto graphVisualsMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::GraphVisualsMode, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, graphVisualsMode));
  grid->Add(0, 7, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, graphVisualsMode));
  grid->MarkSection({ { 0, 0 }, { 1, 8 } });
  auto subSection = plugGUI->StoreComponent<FBSubSectionComponent>(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(subSection);
}

Component*
FFMakeGUISettingsGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::GUISettings].name;
  tabs->addTab(name, {}, MakeSettingsTab(plugGUI), false);
  return tabs;
}