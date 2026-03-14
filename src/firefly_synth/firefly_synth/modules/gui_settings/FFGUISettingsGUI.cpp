#include <firefly_synth/gui/FFPlugGUI.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsGUI.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBLookAndFeel.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBLastTweaked.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
         
using namespace juce;

Component*
FFMakeGUISettingsGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 0, 0, 0, 0, 0, 0 }, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, 1, 3, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "GUI", FBLabelAlign::Left, FBLabelColors::PrimaryForeground));
  grid->MarkSection({ { 0, 0 }, { 1, 3 } }, FBGridSectionMark::DefaultBackground);
  auto hilightMod = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightMod, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, hilightMod));
  grid->Add(1, 1, plugGUI->StoreComponent<FBGUIParamToggleButton>(plugGUI, hilightMod));
  auto hilightTweakMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::HilightTweakMode, 0 } });
  grid->Add(2, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, hilightTweakMode));
  grid->Add(2, 1, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, hilightTweakMode));
  auto knobVisualsMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::KnobVisualsMode, 0 } });
  grid->Add(3, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, knobVisualsMode));
  grid->Add(3, 1, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, knobVisualsMode));
  auto graphVisualsMode = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::GraphVisualsMode, 0 } });
  grid->Add(4, 0, plugGUI->StoreComponent<FBGUIParamLabel>(plugGUI, graphVisualsMode));
  grid->Add(4, 1, plugGUI->StoreComponent<FBGUIParamComboBox>(plugGUI, graphVisualsMode));
  grid->Add(5, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(plugGUI, "Theme"));

  PopupMenu themeMenu = {};
  for (int i = 0; i < plugGUI->Themes().size(); i++)
    themeMenu.addItem(i + 1, plugGUI->Themes()[i].global.name);
  auto themeCombo = plugGUI->StoreComponent<FBAutoSizeComboBox>(plugGUI, themeMenu);
  themeCombo->onChange = [plugGUI, themeCombo] { 
    plugGUI->SwitchTheme(themeCombo->getText().toStdString()); };
  for (int i = 0; i < plugGUI->Themes().size(); i++)
    if (plugGUI->Themes()[i].global.name == plugGUI->HostContext()->ThemeName())
      themeCombo->setSelectedItemIndex(i);
  grid->Add(5, 1, themeCombo);   
  auto card = plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::GUISettings, 0, card);
}