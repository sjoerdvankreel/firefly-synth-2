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
FFMakeMixGUISectionFXToFX(FBPlugGUI* plugGUI, int moduleType, int fxToFXParam, Slider::SliderStyle sliderStyle)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto knobCellSize = sliderStyle == Slider::SliderStyle::LinearHorizontal ? 1 : 0;
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, knobCellSize, 0, knobCellSize, 0, knobCellSize });
  for (int s = 0; s < FFMixFXToFXCount; s++)
  {
    int row = s / (FFMixFXToFXCount / 2);
    int colStart = s % (FFMixFXToFXCount / 2);
    auto mix = topo->audio.ParamAtTopo({ { moduleType, 0 }, { fxToFXParam, s } });
    grid->Add(row, colStart * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
    grid->Add(row, colStart * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, sliderStyle));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

Component*
FFMakeMixGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::MixSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  tabs->AddModuleTab(false, { (int)FFModuleType::VMix, 0 }, FFMakeVMixGUITab(plugGUI));
  tabs->AddModuleTab(false, { (int)FFModuleType::GMix, 0 }, FFMakeGMixGUITab(plugGUI));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}