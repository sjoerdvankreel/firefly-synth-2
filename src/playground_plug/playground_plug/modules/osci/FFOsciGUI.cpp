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

Component&
FFMakeOsciGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  IFBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& grid = FBGUIStoreComponent<FBGridComponent>(store, 1, 7);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamToggleButton>(store, on, hostContext)));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamComboBox>(store, type, hostContext)));
  auto const* note = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamComboBox>(store, note, hostContext)));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, gain, hostContext, Slider::SliderStyle::Rotary)));
  auto const* cent = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, cent, hostContext, Slider::SliderStyle::Rotary)));
  auto const* pw = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, pw, hostContext, Slider::SliderStyle::Rotary)));
  auto const* gLFOToGain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, gLFOToGain, hostContext, Slider::SliderStyle::Rotary)));
  return grid;
}