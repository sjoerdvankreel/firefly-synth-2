#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/lfo/FFLFOGUI.hpp>
#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBMultiLineLabel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component*
MakeLFOSectionMain(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 1, 0, 1, 0, 1 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::On, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto sync = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Sync, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto oneShot = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OneShot, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oneShot));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oneShot));
  auto hostSnap = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::HostSnap, 0 } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hostSnap));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, hostSnap));
  auto phase = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Phase, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, phase));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, phase, Slider::SliderStyle::LinearHorizontal));
  auto steps = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Steps, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, steps));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, steps, Slider::SliderStyle::LinearHorizontal));
  auto seed = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Seed, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::LinearHorizontal));
  auto smoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SmoothTime, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SmoothBars, 0 } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  grid->MarkSection({ { 0, 0 }, { 2, 8 } });
  return grid;
}

static Component*
MakeLFOSectionBlock(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot, int block)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, block } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type, std::string(1, static_cast<char>('A' + block))));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto opKind = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpKind, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, opKind));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opKind));
  auto stack = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Stack, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stack));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, stack));
  auto min = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Min, block } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, min));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, min, Slider::SliderStyle::RotaryVerticalDrag));
  auto max = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Max, block } });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, max));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, max, Slider::SliderStyle::RotaryVerticalDrag));
  auto skewXMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewXMode, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewXMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewXMode));
  auto skewXAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewXAmt, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewXAmt));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skewXAmt, Slider::SliderStyle::RotaryVerticalDrag));
  auto skewYMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewYMode, block } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewYMode));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewYMode));
  auto skewYAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewYAmt, block } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewYAmt));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skewYAmt, Slider::SliderStyle::RotaryVerticalDrag));
  auto rateTime = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateTime, block } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rateTime));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, rateTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto rateBars = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateBars, block } });
  grid->Add(1, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rateBars));
  grid->Add(1, 9, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, rateBars));
  grid->MarkSection({ { 0, 0 }, { 2, 10 } });
  return grid;
}

static Component*
MakeLFOTab(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  columnSizes.push_back(1);
  for (int i = 0; i < FFLFOBlockCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeLFOSectionMain(plugGUI, moduleType, moduleSlot));
  for (int i = 0; i < FFLFOBlockCount; i++)
    grid->Add(0, 1 + i, MakeLFOSectionBlock(plugGUI, moduleType, moduleSlot, i));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeLFOGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::LFOSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  for (int i = 0; i < FFLFOCount; i++)
    tabs->AddModuleTab(i != 0, { (int)FFModuleType::VLFO, i }, MakeLFOTab(plugGUI, FFModuleType::VLFO, i));
  for (int i = 0; i < FFLFOCount; i++)
    tabs->AddModuleTab(i != 0, { (int)FFModuleType::GLFO, i }, MakeLFOTab(plugGUI, FFModuleType::GLFO, i));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}