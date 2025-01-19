#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamComboBox.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeGFilterGUI(
  FBRuntimeTopo const* topo, FBGUIStore* store,
  IFBHostGUIContext* hostContext, Component* root, 
  int moduleSlot)
{
  auto& result = store->AddComponent<FBGridComponent>(1, 5);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0 });
  result.Add(store->AddParamControl<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  result.Add(store->AddParamControl<FBParamComboBox>(type, hostContext));
  auto const* freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(freq, hostContext, root, Slider::SliderStyle::Rotary));
  auto const* res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(res, hostContext, root, Slider::SliderStyle::Rotary));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(gain, hostContext, root, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeGFilterGUI(
  FBRuntimeTopo const* topo, FBGUIStore* store, 
  IFBHostGUIContext* hostContext, Component* root)
{
  return store->AddComponent<FBModuleTabComponent>(topo, store, hostContext, root, (int)FFModuleType::GFilter, MakeGFilterGUI);
}