#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <playground_plug/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBGUIParamLabel.hpp>
#include <playground_base/gui/controls/FBGUIParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0 });
  auto track = topo->gui.ParamAtTopo({ (int)FFModuleType::GUISettings, moduleSlot, (int)FFGUISettingsGUIParam::GraphTrack, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, track));
  grid->Add(0, 1, plugGUI->StoreComponent<FBGUIParamToggleButton>(plugGUI, track));
  grid->MarkSection({ 0, 0, 1, 2 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto all = MakeSectionAll(plugGUI, moduleSlot);
  return plugGUI->StoreComponent<FBSectionComponent>(all);
}

Component*
FFMakeGUISettingsGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::GUISettings, TabFactory);
}