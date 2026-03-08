#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixTopo.hpp>
#include <firefly_synth/modules/mix/FFVMixGUI.hpp>
#include <firefly_synth/modules/mix/FFGMixGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

Component*
FFMakeMixGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto moduleParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::MixSelectedTab, 0 } });
  auto select = plugGUI->StoreComponent<FBSelectComponent>(plugGUI, moduleParam, std::vector<int> { 1, 1 }, std::vector<int> { 1 });
  select->AddSelector(0, 0, { (int)FFModuleType::VMix, 0 }, "VMix", FFMakeVMixGUITab(plugGUI));
  select->AddSelector(1, 0, { (int)FFModuleType::GMix, 0 }, "GMix", FFMakeGMixGUITab(plugGUI));
  select->ActivateStoredSelection();
  return plugGUI->StoreComponent<FBThemedComponent>(plugGUI, (int)FFThemedComponentId::MixSelector, select);
}