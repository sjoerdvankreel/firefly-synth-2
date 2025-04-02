#include <playground_plug/modules/osci_mod/FFOsciModGUI.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>

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
  columnSizes.push_back(0);
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    columnSizes.push_back(0);
    columnSizes.push_back(1);
    columnSizes.push_back(1);
  }
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, columnSizes);
  auto am0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::AM, 0 });
  auto rm0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::RM, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(am0->static_.name + "/" + rm0->static_.name));
  auto tz0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::TZ, 0 });
  auto fm0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::FM, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>(tz0->static_.name + "/" + fm0->static_.name));
  grid->MarkSection({0, 0, 2, 1});
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::On, i });
    grid->Add(0, 1 + i * 3, plugGUI->StoreComponent<FBAutoSizeLabel>(FFOsciModFormatSlot(i)));
    grid->Add(1, 1 + i * 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
    auto am = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::AM, i });
    grid->Add(0, 1 + i * 3 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, am, Slider::SliderStyle::RotaryVerticalDrag));
    auto rm = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::RM, i });
    grid->Add(0, 1 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, rm, Slider::SliderStyle::RotaryVerticalDrag));
    auto tz = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::TZ, i });
    grid->Add(1, 1 + i * 3 + 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, tz));
    auto fm = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::FM, i });
    grid->Add(1, 1 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fm, Slider::SliderStyle::RotaryVerticalDrag));
    grid->MarkSection({ 0, 1 + i * 3 , 2, 3 });
  }
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeOsciModGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::OsciMod, MakeSectionAll);
}