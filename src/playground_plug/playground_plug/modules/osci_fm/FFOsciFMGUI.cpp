#include <playground_plug/modules/osci_fm/FFOsciFMGUI.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMTopo.hpp>

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
FFMakeOsciFMGUI(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, FFMakeOsciModUIColumnSizes());
  auto index0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciFM, 0, (int)FFOsciFMParam::Index, 0 });
  auto indexHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(index0->static_.name);
  grid->Add(0, 0, indexHeader);
  auto throughZero0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciFM, 0, (int)FFOsciFMParam::ThroughZero, 0 });
  auto throughZeroHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(throughZero0->static_.name);
  grid->Add(1, 0, throughZeroHeader);
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciFM, 0, (int)FFOsciFMParam::On, i });
    grid->Add(0, 1 + i * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
    grid->Add(1, 1 + i * 2, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
    auto index = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciFM, 0, (int)FFOsciFMParam::Index, i });
    grid->Add(0, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, index, Slider::SliderStyle::RotaryVerticalDrag));
    auto throughZero = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciFM, 0, (int)FFOsciFMParam::ThroughZero, i });
    grid->Add(1, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, throughZero));
  }
  FFMarkOsciModUIGridSections(grid);
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}