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

Component&
FFMakeGFilterGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& grid = store->AddComponent<FBGridComponent>(1, 5);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0 });
  grid.Add(store->AddParamControl<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  grid.Add(store->AddParamControl<FBParamComboBox>(type, hostContext));
  auto const* freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  grid.Add(store->AddParamControl<FBParamSlider>(freq, hostContext, Slider::SliderStyle::Rotary));
  auto const* res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  grid.Add(store->AddParamControl<FBParamSlider>(res, hostContext, Slider::SliderStyle::Rotary));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  grid.Add(store->AddParamControl<FBParamSlider>(gain, hostContext, Slider::SliderStyle::Rotary));
  auto& tabs = store->AddComponent<TabbedComponent>(TabbedButtonBar::Orientation::TabsAtTop);
  tabs.addTab("TODO", Colours::black, &grid, false);
  return tabs;
}