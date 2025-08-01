#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeGUISettingsTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 1, 0, 1 });
  auto graphRenderMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::GraphRenderMode, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, graphRenderMode));
  grid->Add(0, 1, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, graphRenderMode));
  auto knobRenderMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::KnobRenderMode, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, knobRenderMode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, knobRenderMode));
  grid->MarkSection({ { 0, 0 }, { 1, 4 } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeGUISettingsGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::GUISettings].name;
  tabs->addTab(name, {}, MakeGUISettingsTab(plugGUI), false);
  return tabs;
}