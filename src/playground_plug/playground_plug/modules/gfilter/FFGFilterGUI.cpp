#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamComboBox.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>

using namespace juce;

static Component&
MakeGFilterGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& result = store->AddComponent<FBGridComponent>(1, 5);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0 });
  result.Add(store->AddParamControl<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  result.Add(store->AddParamControl<FBParamComboBox>(type, hostContext));
  auto const* freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(freq, hostContext, Slider::SliderStyle::Rotary));
  auto const* res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(res, hostContext, Slider::SliderStyle::Rotary));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  result.Add(store->AddParamControl<FBParamSlider>(gain, hostContext, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeGFilterGUI(
  FBRuntimeTopo const* topo,
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{;
  auto& result = store->AddComponent<TabbedComponent>(TabbedButtonBar::Orientation::TabsAtTop);
  for (int i = 0; i < FFGFilterCount; i++)
  {
    auto& tab = MakeGFilterGUI(topo, i, store, hostContext);
    result.addTab(std::to_string(i + 1), Colours::black, &tab, false);
  }
  return result;
}