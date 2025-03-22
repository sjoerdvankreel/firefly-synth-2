#include <playground_plug/modules/osci_am/FFOsciAMGUI.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

Component*
FFMakeOsciAMGUI(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, FFMakeOsciModUIColumnSizes());
  auto mix0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Mix, 0 });
  auto mixHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(mix0->static_.name);
  grid->Add(0, 0, mixHeader);
  auto ring0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Ring, 0 });
  auto ringHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(ring0->static_.name);
  grid->Add(1, 0, ringHeader);
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::On, i });
    grid->Add(0, 1 + i * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
    grid->Add(1, 1 + i * 2, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
    auto mix = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Mix, i });
    grid->Add(0, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
    auto ring = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Ring, i });
    grid->Add(1, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ring, Slider::SliderStyle::RotaryVerticalDrag));
  }
  FFMarkOsciModUIGridSections(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}