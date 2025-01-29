#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto on = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto type = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Type, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto sustain = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sustain, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sustain));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, sustain, Slider::SliderStyle::RotaryVerticalDrag));
  auto sync = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Sync, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto exp = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Exp, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, exp));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, exp));
  auto smooth = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Smooth, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, smooth));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, smooth, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 1, 6 });
  grid->MarkSection({ 1, 0, 1, 6 });
  return grid;
}

static Component*
MakeSectionDH(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, std::vector<int> { 0, 0 });
  auto delayTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DelayTime, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayTime));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delayTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto holdTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::HoldTime, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, holdTime));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, holdTime, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 1, 2 });
  grid->MarkSection({ 1, 0, 1, 2 });
  return grid;
}

static Component*
MakeSectionADR(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, std::vector<int> { 0, 1, 0, 1, 0, 1 });
  auto attackTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackTime, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, attackTime));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, attackTime, Slider::SliderStyle::LinearHorizontal));
  auto decayTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecayTime, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decayTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decayTime, Slider::SliderStyle::LinearHorizontal));
  auto releaseTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseTime, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, releaseTime));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, releaseTime, Slider::SliderStyle::LinearHorizontal));
  auto attackSlope = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackSlope, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, attackSlope));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, attackSlope, Slider::SliderStyle::LinearHorizontal));
  auto decaySlope = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecaySlope, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decaySlope));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decaySlope, Slider::SliderStyle::LinearHorizontal));
  auto releaseSlope = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseSlope, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, releaseSlope));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, releaseSlope, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 1, 6 });
  grid->MarkSection({ 1, 0, 1, 6 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionDH(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeSectionADR(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}

Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Env, TabFactory);
}