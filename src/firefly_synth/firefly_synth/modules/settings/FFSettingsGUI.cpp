#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/settings/FFSettingsGUI.hpp>
#include <firefly_synth/modules/settings/FFSettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBButton.hpp>
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
MakeSettingsMain(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0 });
  auto visualsMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::Settings, 0 }, { (int)FFSettingsGUIParam::VisualsMode, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, visualsMode));
  grid->Add(1, 0, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, visualsMode));
  grid->MarkSection({ { 0, 0 }, { 2, 1 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

Component*
FFMakeSettingsGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0 });
  grid->Add(0, 0, MakeSettingsMain(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}