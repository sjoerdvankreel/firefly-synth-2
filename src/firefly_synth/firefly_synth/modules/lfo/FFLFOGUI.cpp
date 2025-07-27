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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto smoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SmoothTime, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SmoothBars, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  auto seed = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Seed, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));
  auto sync = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Sync, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return grid;
}

static Component*
MakeLFOSectionBlock(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot, int block)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto opType = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::OpType, block } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, opType, std::string(1, static_cast<char>('A' + block))));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, opType));
  auto steps = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Steps, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, steps));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, steps, Slider::SliderStyle::RotaryVerticalDrag));
  auto waveMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::WaveMode, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, waveMode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, waveMode));
  auto rateHz = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateHz, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rateHz));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, rateHz, Slider::SliderStyle::RotaryVerticalDrag));
  auto rateBars = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::RateBars, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, rateBars));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, rateBars));
  auto min = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Min, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, min));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, min, Slider::SliderStyle::RotaryVerticalDrag));
  auto max = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::Max, block } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, max));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, max, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return grid;
}

static Component*
MakeLFOSectionSkewA(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0 });
  auto skewAXMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAXMode, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewAXMode));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewAXMode));
  auto skewAXAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAXAmt, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skewAXAmt, Slider::SliderStyle::RotaryVerticalDrag));
  auto skewAYMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAYMode, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewAYMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewAYMode));
  auto skewAYAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::SkewAYAmt, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, skewAYAmt, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 3 } });
  return grid;
}

static Component*
MakeLFOSectionPhaseB(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0 });
  auto phaseB = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFLFOParam::PhaseB, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, phaseB));
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamSlider>(plugGUI, phaseB, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 1 } });
  return grid;
}

static Component*
MakeLFOTab(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  columnSizes.push_back(1);
  columnSizes.push_back(0);
  columnSizes.push_back(0);
  for (int i = 0; i < FFLFOBlockCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeLFOSectionMain(plugGUI, moduleType, moduleSlot));
  grid->Add(0, 1, MakeLFOSectionBlock(plugGUI, moduleType, moduleSlot, 0));
  grid->Add(0, 2, MakeLFOSectionSkewA(plugGUI, moduleType, moduleSlot));
  grid->Add(0, 3, MakeLFOSectionBlock(plugGUI, moduleType, moduleSlot, 1));
  grid->Add(0, 4, MakeLFOSectionPhaseB(plugGUI, moduleType, moduleSlot));
  grid->Add(0, 5, MakeLFOSectionBlock(plugGUI, moduleType, moduleSlot, 2));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeLFOGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::LFOSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  for (int i = 0; i < FFLFOAndEnvCount; i++)
    tabs->AddModuleTab(i != 0, { (int)FFModuleType::VLFO, i }, MakeLFOTab(plugGUI, FFModuleType::VLFO, i));
  for (int i = 0; i < FFLFOAndEnvCount; i++)
    tabs->AddModuleTab(i != 0, { (int)FFModuleType::GLFO, i }, MakeLFOTab(plugGUI, FFModuleType::GLFO, i));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}