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
MakeSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1 });
  auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto shaperGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::ShaperGain, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, shaperGain));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, shaperGain, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 2, 2 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 1 });
  grid->Add(0, 0, MakeSectionAll(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeEffectGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Effect, TabFactory);
}