#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

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
MakeSectionPW(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 1 });
  auto pw = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(pw));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamSlider>(pw, plugGUI, hostContext, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}

static Component*
MakeSectionMain(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto on = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(on));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto type = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(type));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamComboBox>(type, hostContext));
  auto note = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  grid->Add(0, 4, plugGUI->AddComponent<FBParamLabel>(note));
  grid->Add(0, 5, plugGUI->AddComponent<FBParamComboBox>(note, hostContext));
  auto cent = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  grid->Add(0, 6, plugGUI->AddComponent<FBParamLabel>(cent));
  grid->Add(0, 7, plugGUI->AddComponent<FBParamSlider>(cent, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}

static Component*
MakeSectionGain(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto gain1 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(gain1));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamSlider>(gain1, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto gain2 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 1 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(gain2));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamSlider>(gain2, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto gLFOToGain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  grid->Add(0, 4, plugGUI->AddComponent<FBParamLabel>(gLFOToGain));
  grid->Add(0, 5, plugGUI->AddComponent<FBParamSlider>(gLFOToGain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}

static Component*
TabFactory(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto result = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 1 });
  result->Add(0, 0, MakeSectionMain(topo, plugGUI, hostContext, moduleSlot));
  result->Add(0, 1, MakeSectionGain(topo, plugGUI, hostContext, moduleSlot));
  result->Add(0, 2, MakeSectionPW(topo, plugGUI, hostContext, moduleSlot));
  return result;
}

Component*
FFMakeOsciGUI(
  FBRuntimeTopo const* topo,
  FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(
    topo, plugGUI, hostContext, (int)FFModuleType::Osci, TabFactory);
}