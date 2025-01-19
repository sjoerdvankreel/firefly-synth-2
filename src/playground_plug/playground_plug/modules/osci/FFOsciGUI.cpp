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
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeOsciGUI(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = plugGUI->AddComponent<FBGridComponent>(1, 16);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(on));
  result.Add(plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(type));
  result.Add(plugGUI->AddComponent<FBParamComboBox>(type, hostContext));
  auto const* note = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(note));
  result.Add(plugGUI->AddComponent<FBParamComboBox>(note, hostContext));
  auto const* gain1 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(gain1));
  result.Add(plugGUI->AddComponent<FBParamSlider>(gain1, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* gain2 = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 1 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(gain2));
  result.Add(plugGUI->AddComponent<FBParamSlider>(gain2, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* cent = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(cent));
  result.Add(plugGUI->AddComponent<FBParamSlider>(cent, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* pw = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(pw));
  result.Add(plugGUI->AddComponent<FBParamSlider>(pw, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* gLFOToGain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(gLFOToGain));
  result.Add(plugGUI->AddComponent<FBParamSlider>(gLFOToGain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeOsciGUI(
  FBRuntimeTopo const* topo, 
  FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(topo, plugGUI, hostContext, (int)FFModuleType::Osci, MakeOsciGUI);
}