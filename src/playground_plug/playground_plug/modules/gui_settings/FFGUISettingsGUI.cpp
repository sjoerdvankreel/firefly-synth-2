#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBLabel.hpp>
#include <playground_base/gui/controls/FBComboBox.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBTabComponent.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  auto graphMode = topo->gui.ParamAtTopo({ (int)FFModuleType::GUISettings, moduleSlot, (int)FFGUISettingsGUIParam::GraphMode, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, graphMode));
  grid->Add(0, 1, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, graphMode));
  grid->MarkSection({ 0, 0, 1, 2 });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeGUISettingsGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::GUISettings, TabFactory);
}