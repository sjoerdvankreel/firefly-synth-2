#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBParamLabel.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeMasterGUI(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = plugGUI->AddComponent<FBGridComponent>(1, (int)FFMasterParam::Count * 2);
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Gain, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(gain));
  result.Add(plugGUI->AddParamControl<FBParamSlider>(gain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* smooth = topo->ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Smoothing, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(smooth));
  result.Add(plugGUI->AddParamControl<FBParamSlider>(smooth, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return result;
} 

Component&
FFMakeMasterGUI(
  FBRuntimeTopo const* topo, 
  FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(topo, plugGUI, hostContext, (int)FFModuleType::Master, MakeMasterGUI);
}