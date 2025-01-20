#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

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
MakeSectionMain(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& grid = plugGUI->AddComponent<FBGridComponent>(1, 4);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(on));
  grid.Add(plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(type));
  grid.Add(plugGUI->AddComponent<FBParamComboBox>(type, hostContext));
  return plugGUI->AddComponent<FBSectionComponent>(&grid);
}

static Component&
MakeSectionParams(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& grid = plugGUI->AddComponent<FBGridComponent>(1, 6);
  auto const* freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(freq));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(freq, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(res));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(res, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  grid.Add(plugGUI->AddComponent<FBParamLabel>(gain));
  grid.Add(plugGUI->AddComponent<FBParamSlider>(gain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(&grid);
}
  
static Component&
TabFactory(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = plugGUI->AddComponent<FBGridComponent>(1, 2);
  result.Add(MakeSectionMain(topo, plugGUI, hostContext, moduleSlot));
  result.Add(MakeSectionParams(topo, plugGUI, hostContext, moduleSlot));
  return result;
}

Component&
FFMakeGFilterGUI(
  FBRuntimeTopo const* topo,
  FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(topo, plugGUI, hostContext, (int)FFModuleType::GFilter, TabFactory);
}