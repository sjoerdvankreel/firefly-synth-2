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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, -1, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Type, block });
  grid->Add(0, 0, 1, 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto shaperGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::ShaperGain, block });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, shaperGain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, shaperGain, Slider::SliderStyle::RotaryVerticalDrag));
  auto combKeyTrk = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombKeyTrk, block });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combKeyTrk));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combKeyTrk, Slider::SliderStyle::RotaryVerticalDrag));
  auto combFreqPlus = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombFreqPlus, block });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqPlus));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combResPlus = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombResPlus, block });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResPlus));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combFreqMin = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombFreqMin, block });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqMin));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqMin, Slider::SliderStyle::RotaryVerticalDrag));
  auto combResMin = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::CombResMin, block });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResMin));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResMin, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 6 });
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