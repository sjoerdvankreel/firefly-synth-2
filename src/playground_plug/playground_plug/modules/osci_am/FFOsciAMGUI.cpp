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

static Component*
MakeSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  columnSizes.push_back(0);
  for (int i = 0; i < FFOsciAMSlotCount; i++)
  {
    columnSizes.push_back(0);
    columnSizes.push_back(1);
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, columnSizes);
  auto mix0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Mix, 0 });
  auto mixHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(mix0->static_.name);
  grid->Add(0, 0, mixHeader);
  auto ring0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Ring, 0 });
  auto ringHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(ring0->static_.name);
  grid->Add(1, 0, ringHeader);
  for (int i = 0; i < FFOsciAMSlotCount; i++)
  {
    auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::On, i });
    grid->Add(0, 1 + i * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
    grid->Add(1, 1 + i * 2, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
    auto mix = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Mix, i });
    grid->Add(0, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
    auto ring = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Ring, i });
    grid->Add(1, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ring, Slider::SliderStyle::RotaryVerticalDrag));
  }
  int x = 0;
  int w = 2;
  grid->MarkSection({ 0, 0, 2, 1 });
  for (int i = 0; i < FFOsciCount; i++)
  {
    grid->MarkSection({ 0, 1 + x, 2, w });
    x += 2 * (i + 1);
    w += 2;
  }
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeOsciAMGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::OsciAM, MakeSectionAll);
}