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
  auto amRmHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(am0->static_.name + "/" + rm0->static_.name);
  grid->Add(0, 0, amRmHeader);
  auto fm0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::FM, 0 });
  auto tzfm0 = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::TZFM, 0 });
  auto fmTzfmHeader = plugGUI->StoreComponent<FBAutoSizeLabel>(fm0->static_.name + "/" + tzfm0->static_.name);
  grid->Add(1, 0, fmTzfmHeader);
  grid->MarkSection({0, 0, 2, 1});
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::On, i });
    grid->Add(0, 1 + i * 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
    grid->Add(1, 1 + i * 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
    auto am = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::AM, i });
    grid->Add(0, 1 + i * 3 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, am, Slider::SliderStyle::RotaryVerticalDrag));
    auto rm = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::RM, i });
    grid->Add(0, 1 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, rm, Slider::SliderStyle::RotaryVerticalDrag));
    auto fm = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::FM, i });
    grid->Add(1, 1 + i * 3 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fm, Slider::SliderStyle::RotaryVerticalDrag));
    auto tzfm = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::TZFM, i });
    grid->Add(1, 1 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, tzfm, Slider::SliderStyle::RotaryVerticalDrag));
    grid->MarkSection({ 0, 1 + i * 3 , 2, 3 });
  }
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeOsciModGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::OsciMod, MakeSectionAll);
}