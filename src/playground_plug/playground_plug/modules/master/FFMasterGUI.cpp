#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>

using namespace juce;

Component&
FFMakeMasterGUI(
  FBRuntimeTopo const* topo,   
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& grid = store->AddComponent<FBGridComponent>(1, 2);
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::Master, 0, (int)FFMasterParam::Gain, 0 });
  grid.Add(store->AddParamControl<FBParamSlider>(gain, hostContext, Slider::SliderStyle::Rotary));
  auto const* smooth = topo->ParamAtTopo({ (int)FFModuleType::Master, 0, (int)FFMasterParam::Smoothing, 0 });
  grid.Add(store->AddParamControl<FBParamSlider>(smooth, hostContext, Slider::SliderStyle::Rotary));
  return grid;
} 