#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeGLFOGUI(
  FBRuntimeTopo const* topo, FBGUIStore* store, 
  IFBHostGUIContext* hostContext, Component* root,
  int moduleSlot)
{
  auto& result = store->AddComponent<FBGridComponent>(1, 2);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::On, 0 });
  result.Add(store->AddParamControl<FBParamToggleButton>(on, hostContext));
  auto const* rate = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(rate, hostContext, root, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeGLFOGUI(
  FBRuntimeTopo const* topo, FBGUIStore* store, 
  IFBHostGUIContext* hostContext, Component* root)
{
  return store->AddComponent<FBModuleTabComponent>(topo, store, hostContext, root, (int)FFModuleType::GLFO, MakeGLFOGUI);
}