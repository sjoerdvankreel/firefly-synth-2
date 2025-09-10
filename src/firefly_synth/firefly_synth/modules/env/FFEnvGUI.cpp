#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/env/FFEnvGUI.hpp>
#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeEnvSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 1, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Type, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto smoothTime = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothTime, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::SmoothBars, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  auto release = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Release, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, release));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, release));
  auto sync = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::Sync, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto loopStart = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopStart, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, loopStart));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, loopStart));
  auto loopLength = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::LoopLength, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, loopLength));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, loopLength));
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEnvSectionStage(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = {};
  columnSizes.push_back(0);
  for (int i = 0; i < FFEnvStageCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Stage Length"));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Level/Slope"));
  grid->MarkSection({ { 0, 0 }, { 2, 1 } });
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    auto upper = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
    grid->Add(0, 1 + i, upper);
    auto time = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageTime, i } });
    upper->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, time, std::to_string(i + 1)));
    upper->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, time, Slider::SliderStyle::LinearHorizontal));
    auto bars = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageBars, i } });
    upper->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bars, std::to_string(i + 1)));
    upper->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bars));
    auto lower = plugGUI->StoreComponent<FBGridComponent>(true, 0, -1, std::vector<int> { 1 }, std::vector<int> { 1, 1 });
    grid->Add(1, 1 + i, lower);
    auto level = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot} , { (int)FFEnvParam::StageLevel, i } });
    lower->Add(0, 0, plugGUI->StoreComponent<FBParamSlider>(plugGUI, level, Slider::SliderStyle::RotaryVerticalDrag));
    auto slope = topo->audio.ParamAtTopo({ { (int)FFModuleType::Env, moduleSlot }, { (int)FFEnvParam::StageSlope, i } });
    lower->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, slope, Slider::SliderStyle::RotaryVerticalDrag));
    grid->MarkSection({ { 0, 1 + i }, { 2, 1 } });
  }
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeEnvTab(FBPlugGUI* plugGUI, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 1, 0 });
  grid->Add(0, 0, MakeEnvSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeEnvSectionStage(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::EnvSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  tabs->SetTabSeparatorText(0, "Env");
  tabs->AddModuleTab(true, false, { (int)FFModuleType::Env, FFAmpEnvSlot }, MakeEnvTab(plugGUI, FFAmpEnvSlot));
  for (int i = FFEnvSlotOffset; i < FFEnvCount; i++)
    tabs->AddModuleTab(true, false, { (int)FFModuleType::Env, i }, MakeEnvTab(plugGUI, i));
  tabs->ActivateStoredSelectedTab();

  PopupMenu insertMenu;
  PopupMenu removeMenu;
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    insertMenu.addItem(1000 + i, std::to_string(i + 1));
    removeMenu.addItem(2000 + i, std::to_string(i + 1));
  }
  tabs->extendedMenu.addSubMenu("Remove Stage At", removeMenu);
  tabs->extendedMenu.addSubMenu("Insert Stage Before", insertMenu);
  tabs->extendedMenuHandler = [](FBPlugGUI* plugGUI, FBTopoIndices const& indices, int id) {

    std::vector<FFEnvParam> stageParams = { 
      FFEnvParam::StageTime, FFEnvParam::StageBars, 
      FFEnvParam::StageLevel, FFEnvParam::StageSlope };

    if (1000 <= id && id < 1000 + FFEnvStageCount)
    {
      int stage = id - 1000;
      plugGUI->HostContext()->UndoState().Snapshot("Insert Stage Before " + std::to_string(stage + 1));
      for (int i = FFEnvStageCount - 1; i > stage; i++)
        for (int j = 0; j < stageParams.size(); j++)
          plugGUI->HostContext()->CopyAudioParam(
            { indices, { (int)stageParams[j], i - 1 } }, { indices, { (int)stageParams[j], i } });
    }

    else if (2000 <= id && id < 2000 + FFEnvStageCount)
    {
      int stage = id - 2000;
      plugGUI->HostContext()->UndoState().Snapshot("Remove Stage At " + std::to_string(stage + 1));
      for (int i = stage; i < FFEnvStageCount - 1; i++)
        for (int j = 0; j < stageParams.size(); j++)
          plugGUI->HostContext()->CopyAudioParam(
            { indices, { (int)stageParams[j], i + 1 } }, { indices, { (int)stageParams[j], i } });
    }
  };

  return tabs;
}