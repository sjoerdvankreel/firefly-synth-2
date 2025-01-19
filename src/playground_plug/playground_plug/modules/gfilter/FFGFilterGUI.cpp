#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBParamLabel.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamComboBox.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeGFilterGUI(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = plugGUI->AddComponent<FBGridComponent>(1, (int)FFGFilterParam::Count * 2);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(on));
  result.Add(plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(type));
  result.Add(plugGUI->AddComponent<FBParamComboBox>(type, hostContext));
  auto const* freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(freq));
  result.Add(plugGUI->AddComponent<FBParamSlider>(freq, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(res));
  result.Add(plugGUI->AddComponent<FBParamSlider>(res, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(gain));
  result.Add(plugGUI->AddComponent<FBParamSlider>(gain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeGFilterGUI(
  FBRuntimeTopo const* topo,
  FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(topo, plugGUI, hostContext, (int)FFModuleType::GFilter, MakeGFilterGUI);
}