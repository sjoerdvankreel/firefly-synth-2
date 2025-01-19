#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBParamLabel.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeGLFOGUI(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = plugGUI->AddComponent<FBGridComponent>(1, (int)FFGLFOParam::Count * 2);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::On, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(on));
  result.Add(plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto const* rate = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 });
  result.Add(plugGUI->AddComponent<FBParamLabel>(rate));
  result.Add(plugGUI->AddComponent<FBParamSlider>(rate, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeGLFOGUI(
  FBRuntimeTopo const* topo, 
  FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(topo, plugGUI, hostContext, (int)FFModuleType::GLFO, MakeGLFOGUI);
}