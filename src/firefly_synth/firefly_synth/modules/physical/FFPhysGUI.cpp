#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/physical/FFPhysGUI.hpp>
#include <firefly_synth/modules/physical/FFPhysTopo.hpp>

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
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Type, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto gain = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Gain, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  auto coarse = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Coarse, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, coarse));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, coarse, Slider::SliderStyle::RotaryVerticalDrag));
  auto fine = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Fine, 0 });
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
  auto count = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::UniCount, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, count));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, count, Slider::SliderStyle::RotaryVerticalDrag));
  auto detune = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::UniDetune, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, detune));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, detune, Slider::SliderStyle::RotaryVerticalDrag));
  auto spread = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::UniSpread, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, spread));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, spread, Slider::SliderStyle::RotaryVerticalDrag));
  auto blend = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::UniBlend, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, blend));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, blend, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 4 });
  return grid;
}

static Component*
MakeSectionNoise(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto seed = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Seed, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, seed));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, seed, Slider::SliderStyle::RotaryVerticalDrag));  
  auto excite = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Excite, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, excite));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, excite, Slider::SliderStyle::RotaryVerticalDrag));  
  auto color = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Color, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, color));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, color, Slider::SliderStyle::RotaryVerticalDrag));
  auto poles = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Poles, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, poles));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, poles, Slider::SliderStyle::RotaryVerticalDrag));
  auto x = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::X, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, x));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, x, Slider::SliderStyle::RotaryVerticalDrag));
  auto y = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Y, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, y));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, y, Slider::SliderStyle::RotaryVerticalDrag));  
  grid->MarkSection({ 0, 0, 2, 6 });
  return grid;
}

static Component*
MakeSectionKS(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 1, 0, 0, 0, 0 });
  auto lp = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::LP, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lp));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lp, Slider::SliderStyle::LinearHorizontal));
  auto hp = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::HP, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hp));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hp, Slider::SliderStyle::LinearHorizontal));
  auto damp = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Damp, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, damp));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, damp, Slider::SliderStyle::LinearHorizontal));
  auto dampScale = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::DampScale, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dampScale));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dampScale, Slider::SliderStyle::RotaryVerticalDrag));
  auto feedback = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Feedback, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedback));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedback, Slider::SliderStyle::LinearHorizontal));
  auto feedbackScale = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::FeedbackScale, 0 });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, feedbackScale));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, feedbackScale, Slider::SliderStyle::RotaryVerticalDrag));
  auto center = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Center, 0 });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, center));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, center, Slider::SliderStyle::RotaryVerticalDrag));
  auto range = topo->audio.ParamAtTopo({ (int)FFModuleType::Phys, moduleSlot, (int)FFPhysParam::Range, 0 });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, range));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, range, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 2, 8 });
  return grid;
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 1 });
  grid->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  grid->Add(0, 1, MakeSectionUni(plugGUI, moduleSlot));
  grid->Add(0, 2, MakeSectionNoise(plugGUI, moduleSlot));
  grid->Add(0, 3, MakeSectionKS(plugGUI, moduleSlot));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakePhysGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Phys, TabFactory);
}