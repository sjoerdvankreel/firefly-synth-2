#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeMasterGUI(
  FBRuntimeTopo const* topo, FBGUIStore* store, 
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = store->AddComponent<FBGridComponent>(1, 2);
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Gain, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(gain, hostContext, Slider::SliderStyle::Rotary));
  auto const* smooth = topo->ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Smoothing, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(smooth, hostContext, Slider::SliderStyle::Rotary));
  return result;
} 

Component&
FFMakeMasterGUI(
  FBRuntimeTopo const* topo,
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{
  return store->AddComponent<FBModuleTabComponent>(topo, store, hostContext, (int)FFModuleType::Master, MakeMasterGUI);
}