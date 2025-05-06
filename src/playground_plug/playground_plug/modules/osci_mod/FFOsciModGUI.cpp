#include <playground_plug/modules/osci_mod/FFOsciModGUI.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
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
  columnSizes.push_back(0);
  columnSizes.push_back(0);
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    columnSizes.push_back(0);
    columnSizes.push_back(0);
    columnSizes.push_back(1);
  }
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, columnSizes);
  auto oversample = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::Oversample, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  auto expoFM = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::ExpoFM, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, expoFM));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, expoFM));
  grid->MarkSection({ 0, 0, 2, 2 });

  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>("AM"));
  grid->Add(1, 2, plugGUI->StoreComponent<FBAutoSizeLabel>("FM"));
  grid->MarkSection({ 0, 2, 2, 1 });

  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    grid->Add(0, 3 + i * 3, 2, 1, plugGUI->StoreComponent<FBAutoSizeLabel>(FFOsciModFormatSlot(i)));
    auto amMode = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::AMMode, i });
    grid->Add(0, 3 + i * 3 + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, amMode));
    auto amMix = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::AMMix, i });
    grid->Add(0, 3 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amMix, Slider::SliderStyle::LinearHorizontal));
    auto fmOn = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::FMOn, i });
    grid->Add(1, 3 + i * 3 + 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, fmOn));
    auto fmIndex = topo->audio.ParamAtTopo({ (int)FFModuleType::OsciMod, 0, (int)FFOsciModParam::FMIndex, i });
    grid->Add(1, 3 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex, Slider::SliderStyle::LinearHorizontal));
    grid->MarkSection({ 0, 3 + i * 3 , 2, 3 });
  }
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeOsciModGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::OsciMod, MakeSectionAll);
}