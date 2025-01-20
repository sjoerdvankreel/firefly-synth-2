#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeMasterSectionAll(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 1, 0, 1 } );
  auto gain = topo->ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Gain, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(gain));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamSlider>(gain, plugGUI, hostContext, Slider::SliderStyle::LinearHorizontal));
  auto smooth = topo->ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Smoothing, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(smooth));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamSlider>(smooth, plugGUI, hostContext, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
} 

Component*
FFMakeMasterGUI(
  FBRuntimeTopo const* topo, 
  FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(
    topo, plugGUI, hostContext, (int)FFModuleType::Master, MakeMasterSectionAll);
}