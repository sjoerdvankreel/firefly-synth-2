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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto sync = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sync, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto sustain = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sustain, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sustain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sustain));
  auto release = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Release, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, release));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, release));
  auto smoothTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothTime, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothBars, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  auto mode = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Mode, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int>(FFEnvStageCount * 2));
  for (int i = 0; i < FFEnvStageCount; i++)
  {
    auto time = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageTime, i });
    grid->Add(0, i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, time, Slider::SliderStyle::RotaryVerticalDrag));
    auto bars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageBars, i });
    grid->Add(0, i * 2 + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bars));
    auto level = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageLevel, i });
    grid->Add(1, i * 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, level, Slider::SliderStyle::RotaryVerticalDrag));
    auto slope = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::StageSlope, i });
    grid->Add(1, i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, slope, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ 0, 0, 2, FFEnvStageCount * 2 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionStage(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Env, TabFactory);
}