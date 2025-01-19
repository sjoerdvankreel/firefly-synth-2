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
  IFBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& grid = FBGUIStoreComponent<FBGridComponent>(store, 1, 2);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamToggleButton>(store, on, hostContext)));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamComboBox>(store, type, hostContext)));
  auto const* freq = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, freq, hostContext, Slider::SliderStyle::Rotary)));
  auto const* res = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, res, hostContext, Slider::SliderStyle::Rotary)));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, gain, hostContext, Slider::SliderStyle::Rotary)));
  return grid;
}