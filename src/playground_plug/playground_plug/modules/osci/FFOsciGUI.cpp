#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamComboBox.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>

using namespace juce;

static Component&
MakeOsciGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& result = store->AddComponent<FBGridComponent>(1, 7);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  result.Add(store->AddParamControl<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  result.Add(store->AddParamControl<FBParamComboBox>(type, hostContext));
  auto const* note = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  result.Add(store->AddParamControl<FBParamComboBox>(note, hostContext));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(gain, hostContext, Slider::SliderStyle::Rotary));
  auto const* cent = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(cent, hostContext, Slider::SliderStyle::Rotary));
  auto const* pw = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(pw, hostContext, Slider::SliderStyle::Rotary));
  auto const* gLFOToGain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(gLFOToGain, hostContext, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeOsciGUI(
  FBRuntimeTopo const* topo, 
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& result = store->AddComponent<TabbedComponent>(TabbedButtonBar::Orientation::TabsAtTop);
  for (int i = 0; i < FFOsciCount; i++)
  {
    auto& tab = MakeOsciGUI(topo, i, store, hostContext);
    result.addTab(std::to_string(i + 1), Colours::black, &tab, false);
  }
  return result;
}