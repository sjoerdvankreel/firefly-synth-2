#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/effect/FFEffectGUI.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBLabel.hpp>
#include <playground_base/gui/controls/FBSlider.hpp>
#include <playground_base/gui/controls/FBComboBox.hpp>
#include <playground_base/gui/controls/FBToggleButton.hpp>
#include <playground_base/gui/controls/FBMultiLineLabel.hpp>
#include <playground_base/gui/components/FBTabComponent.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1 });
  auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto oversample = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Oversample, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  grid->MarkSection({ 0, 0, 2, 2 });
  return grid;
}

static Component*
MakeSectionBlock(FBPlugGUI* plugGUI, int moduleSlot, int block)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Type, block });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto shaperGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::ShaperGain, block });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, shaperGain));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, shaperGain, Slider::SliderStyle::RotaryVerticalDrag));
  auto combDelayPlus = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombDelayPlus, block });
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combDelayPlus));
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combDelayPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combGainPlus = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombGainPlus, block });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combGainPlus));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combGainPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combDelayMin = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombDelayMin, block });
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combDelayMin));
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combDelayMin, Slider::SliderStyle::RotaryVerticalDrag));
  auto combGainMin = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombGainMin, block });
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combGainMin));
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combGainMin, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 5 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  std::vector<int> columnSizes = {};
  columnSizes.push_back(1);
  for (int i = 0; i < FFEffectBlockCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  for(int i = 0; i < FFEffectBlockCount; i++)
    grid->Add(0, 1 + i, MakeSectionBlock(plugGUI, moduleSlot, i));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeEffectGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Effect, TabFactory);
}