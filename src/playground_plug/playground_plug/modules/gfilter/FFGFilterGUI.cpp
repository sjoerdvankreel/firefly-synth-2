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

static Component*
MakeSectionMain(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 0 });
  auto on = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(on));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(type));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamComboBox>(type, hostContext));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}

static Component*
MakeSectionParams(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, 6);
  auto freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(freq));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamSlider>(freq, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(res));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamSlider>(res, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  grid->Add(0, 4, plugGUI->AddComponent<FBParamLabel>(gain));
  grid->Add(0, 5, plugGUI->AddComponent<FBParamSlider>(gain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}
  
static Component*
TabFactory(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto result = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 1 });
  result->Add(0, 0, MakeSectionMain(topo, plugGUI, hostContext, moduleSlot));
  result->Add(0, 1, MakeSectionParams(topo, plugGUI, hostContext, moduleSlot));
  return result;
}

Component*
FFMakeGFilterGUI(
  FBRuntimeTopo const* topo,
  FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(
    topo, plugGUI, hostContext, (int)FFModuleType::GFilter, TabFactory);
}