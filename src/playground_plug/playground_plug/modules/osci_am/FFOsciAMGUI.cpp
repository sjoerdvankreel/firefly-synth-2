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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 3, 1 + FFOsciAMSlotCount * 2);
  for (int i = 0; i < FFOsciAMSlotCount; i++)
  {
    auto on = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::On, i });
    grid->Add(0, 1 + i * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
    grid->Add(0, 1 + i * 2 + 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
    auto mix = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Mix, i });
    auto mixHeader = plugGUI->StoreComponent<Label>();
    mixHeader->setText(mix->static_.name, dontSendNotification);
    grid->Add(1, 0, mixHeader);
    grid->Add(1, 1 + i * 2, 1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::LinearHorizontal));
    auto ring = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciAM, 0, (int)FFOsciAMParam::Ring, i });
    auto ringHeader = plugGUI->StoreComponent<Label>();
    ringHeader->setText(ring->static_.name, dontSendNotification);
    grid->Add(2, 0, ringHeader);
    grid->Add(2, 1 + i * 2, 1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ring, Slider::SliderStyle::LinearHorizontal));
  }
  return grid;
}

Component*
FFMakeOsciAMGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::OsciAM, MakeSectionAll);
}