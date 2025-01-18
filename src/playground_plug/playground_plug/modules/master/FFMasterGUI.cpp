#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>

using namespace juce;

Component&
FFMakeMasterGUI(FBRuntimeTopo const* topo, IFBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& grid = FBGUIStoreComponent<FBGridComponent>(store);
  grid.grid.templateRows.add(Grid::TrackInfo(Grid::Fr(1)));
  grid.grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(1)));
  auto const* param = topo->ParamAtTopo({ (int)FFModuleType::Master, 0, (int)FFMasterParam::Gain, 0 });
  grid.AddItemAndChild(GridItem(FBGUIStoreParamControl<FBParamSlider>(store, param, hostContext, Slider::SliderStyle::Rotary)));
  return grid;
} 