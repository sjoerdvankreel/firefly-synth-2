#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>

#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBLabel.hpp>
#include <playground_base/gui/controls/FBSlider.hpp>
#include <playground_base/gui/controls/FBComboBox.hpp>
#include <playground_base/gui/controls/FBToggleButton.hpp>
#include <playground_base/gui/components/FBTabComponent.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 1, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto sync = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sync, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto sustain = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sustain, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sustain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sustain));
  auto exp = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Exp, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, exp));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, exp));
  auto release = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Release, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, release));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, release));
  auto smoothTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothTime, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothBars, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  auto loopStart = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::LoopStart, 0 });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, loopStart));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, loopStart));
  auto loopLength = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::LoopLength, 0 });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, loopLength));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, loopLength));
  grid->MarkSection({ 0, 0, 2, 8 });
  return grid;
}

static Component*
MakeSectionStage(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = {};
  columnSizes.push_back(0);
  for (int i = 0; i < FFEnvStageCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 0, -1, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Seg Len"));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Lvl/Slp"));
  grid->MarkSection({ 0, 0, 2, 1 });
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    auto upper = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 0, -1, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
    grid->Add(0, 1 + i, upper);
    upper->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(i + 1)));
    auto time = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageTime, i });
    upper->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, time, Slider::SliderStyle::LinearHorizontal));
    auto bars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageBars, i });
    upper->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bars));
    auto lower = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 0, -1, std::vector<int> { 1 }, std::vector<int> { 1, 1 });
    grid->Add(1, 1 + i, lower);
    auto level = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageLevel, i });
    lower->Add(0, 0, plugGUI->StoreComponent<FBParamSlider>(plugGUI, level, Slider::SliderStyle::RotaryVerticalDrag));
    auto slope = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageSlope, i });
    lower->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, slope, Slider::SliderStyle::RotaryVerticalDrag));
    grid->MarkSection({ 0, 1 + i, 2, 1 });
  }
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 1, 0 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionStage(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Env, TabFactory);
}