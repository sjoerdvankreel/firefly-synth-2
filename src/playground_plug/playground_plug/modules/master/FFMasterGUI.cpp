#include <playground_plug/modules/master/FFMasterGUI.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>

using namespace juce;

Component&
FFMakeMasterGUI(FBRuntimeTopo const* topo, IFBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& grid = FBGUIStoreComponent<Grid>(store);
  grid.templateRows.add(Grid::TrackInfo(Grid::Fr(1)));
  grid.templateColumns.add(Grid::TrackInfo(Grid::Fr(1)));
  grid.items.add(GridItem(FBGUIStoreParamControl<FBParamSlider>()));
  return grid;
} 