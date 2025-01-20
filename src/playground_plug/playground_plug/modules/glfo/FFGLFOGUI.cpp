#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeGLFOSectionAll(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 1});
  auto on = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::On, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(on));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamToggleButton>(on, hostContext));
  auto rate = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(rate));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamSlider>(rate, plugGUI, hostContext, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}

Component*
FFMakeGLFOGUI(
  FBRuntimeTopo const* topo, 
  FBPlugGUI* plugGUI, 
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(
    topo, plugGUI, hostContext, (int)FFModuleType::GLFO, MakeGLFOSectionAll);
}