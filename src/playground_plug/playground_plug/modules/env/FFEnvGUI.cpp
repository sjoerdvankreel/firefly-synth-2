#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto type = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Type, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto sync = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sync, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto mode = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Mode, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
  grid->MarkSection({ 0, 0, 1, 4 });
  grid->MarkSection({ 1, 0, 1, 4 });
  return grid;
}

static Component*
MakeSectionVertical(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, std::vector<int> { 0, 1, 0, 1 });
  auto attackSlope = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackSlope, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, attackSlope));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, attackSlope, Slider::SliderStyle::LinearHorizontal));
  auto decaySlope = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecaySlope, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decaySlope));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decaySlope, Slider::SliderStyle::LinearHorizontal));
  auto releaseSlope = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseSlope, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, releaseSlope));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, releaseSlope, Slider::SliderStyle::LinearHorizontal));
  auto sustainLevel = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SustainLevel, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sustainLevel));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, sustainLevel, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 1, 4 });
  grid->MarkSection({ 1, 0, 1, 4 });
  return grid;
}

static Component*
MakeSectionHorizontal(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto delayTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DelayTime, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayTime));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delayTime, Slider::SliderStyle::LinearHorizontal));
  auto delayBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DelayBars, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayBars));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, delayBars));
  auto holdTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::HoldTime, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, holdTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, holdTime, Slider::SliderStyle::LinearHorizontal));
  auto holdBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::HoldBars, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, holdBars));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, holdBars));
  auto releaseTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseTime, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, releaseTime));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, releaseTime, Slider::SliderStyle::LinearHorizontal));
  auto releaseBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseBars, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, releaseBars));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, releaseBars));
  auto attackTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackTime, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, attackTime));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, attackTime, Slider::SliderStyle::LinearHorizontal));
  auto attackBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackBars, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, attackBars));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, attackBars));
  auto decayTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecayTime, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decayTime));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decayTime, Slider::SliderStyle::LinearHorizontal));
  auto decayBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecayBars, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decayBars));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, decayBars));
  auto smoothTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothTime, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothTime));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smoothTime, Slider::SliderStyle::LinearHorizontal));
  auto smoothBars = topo->audio.ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SmoothBars, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smoothBars));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, smoothBars));
  grid->MarkSection({ 0, 0, 1, 6 });
  grid->MarkSection({ 1, 0, 1, 6 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionHorizontal(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeSectionVertical(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}

Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Env, TabFactory);
}