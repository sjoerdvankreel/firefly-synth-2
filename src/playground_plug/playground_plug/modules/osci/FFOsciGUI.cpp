#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

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
#include <playground_base/gui/components/FBParamsDependentSectionComponent.hpp>

using namespace juce;

static Component*
MakeSectionPW(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 1 });
  auto pw = topo->ParamAtTopo({(int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, pw));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, pw, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto on = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto type = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto note = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, note));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, note));
  auto cent = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, cent));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, cent, Slider::SliderStyle::RotaryVerticalDrag));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}

static Component*
MakeSectionGain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto gain1 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain1));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain1, Slider::SliderStyle::RotaryVerticalDrag));
  auto gain2 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 1 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain2));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain2, Slider::SliderStyle::RotaryVerticalDrag));
  auto gLFOToGain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gLFOToGain));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gLFOToGain, Slider::SliderStyle::RotaryVerticalDrag));
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  FBParamsDependency dependency = { { (int)FFOsciParam::On }, [](auto const& vs) { return vs[0] != 0; } };
  return plugGUI->StoreComponent<FBParamsDependentSectionComponent>(plugGUI, grid, indices, dependency);
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto result = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 1 });
  result->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  result->Add(0, 1, MakeSectionGain(plugGUI, moduleSlot));
  result->Add(0, 2, MakeSectionPW(plugGUI, moduleSlot));
  return result;
}

Component*
FFMakeOsciGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Osci, TabFactory);
}