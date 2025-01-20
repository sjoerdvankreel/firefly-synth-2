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

static Component&
MakeSectionPW(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& grid = plugGUI->AddComponent<FBGridComponent>(1, 2);
  auto const* pw = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(pw));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(pw, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(&grid);
}

static Component&
MakeSectionMain(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& grid = plugGUI->AddComponent<FBGridComponent>(1, 8);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(on));
  grid.Add(plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(type));
  grid.Add(plugGUI->AddComponent<FBParamComboBox>(type, hostContext));
  auto const* note = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(note));
  grid.Add(plugGUI->AddComponent<FBParamComboBox>(note, hostContext));
  auto const* cent = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(cent));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(cent, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(&grid);
}

static Component&
MakeSectionGain(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& grid = plugGUI->AddComponent<FBGridComponent>(1, 6);
  auto const* gain1 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(gain1));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(gain1, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* gain2 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 1 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(gain2));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(gain2, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* gLFOToGain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(gLFOToGain));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(gLFOToGain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(&grid);
}

static Component&
TabFactory(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = plugGUI->AddComponent<FBGridComponent>(1, 3);
  result.Add(MakeSectionMain(topo, plugGUI, hostContext, moduleSlot));
  result.Add(MakeSectionGain(topo, plugGUI, hostContext, moduleSlot));
  result.Add(MakeSectionPW(topo, plugGUI, hostContext, moduleSlot));
  return result;
}

Component&
FFMakeOsciGUI(
  FBRuntimeTopo const* topo,
  FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(topo, plugGUI, hostContext, (int)FFModuleType::Osci, TabFactory);
}