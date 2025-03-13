#include <playground_plug/modules/osci_am/FFOsciAMGUI.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 3, FFOsciAMSlotCount);
  for (int i = 0; i < FFOsciAMSlotCount; i++)
  {
    auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::On, i });
    grid->Add(0, i, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
    auto mix = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Mix, i });
    grid->Add(1, i, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
    auto ring = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Ring, i });
    grid->Add(2, i, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ring, Slider::SliderStyle::RotaryVerticalDrag));
  }
  return grid;
}

Component*
FFMakeOsciAMGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::OsciAM, MakeSectionAll);
}