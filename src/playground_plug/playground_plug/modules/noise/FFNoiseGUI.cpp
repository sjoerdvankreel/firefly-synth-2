#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/noise/FFNoiseGUI.hpp>
#include <playground_plug/modules/noise/FFNoiseTopo.hpp>

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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto gain = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Gain, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  auto coarse = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Coarse, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, coarse));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, coarse, Slider::SliderStyle::RotaryVerticalDrag));
  auto fine = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Fine, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fine));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fine, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 4 });
  return grid;
}

static Component*
MakeSectionUni(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto count = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::UniCount, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, count));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, count, Slider::SliderStyle::RotaryVerticalDrag));
  auto detune = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::UniDetune, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, detune));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, detune, Slider::SliderStyle::RotaryVerticalDrag));
  auto spread = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::UniSpread, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, spread));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag));
  auto blend = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::UniBlend, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, blend));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, blend, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 4 });
  return grid;
}

static Component*
MakeSectionParams(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 1, 0, 1 });
  auto seed = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Seed, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));
  auto poles = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Poles, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, poles));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, poles, Slider::SliderStyle::RotaryVerticalDrag));
  auto x = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::X, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, x));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, x, Slider::SliderStyle::LinearHorizontal));
  auto y = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Y, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, y));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, y, Slider::SliderStyle::LinearHorizontal));
  auto color = topo->audio.ParamAtTopo({ (int)FFModuleType::Noise, moduleSlot, (int)FFNoiseParam::Color, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, color));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, color, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 2, 6 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionUni(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeSectionParams(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeNoiseGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Noise, TabFactory);
}