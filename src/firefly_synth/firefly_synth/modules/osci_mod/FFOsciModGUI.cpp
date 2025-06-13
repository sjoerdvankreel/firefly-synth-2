#include <firefly_synth/modules/osci_mod/FFOsciModGUI.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/controls/FBMultiLineLabel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

static Component*
MakeOsciModSectionAll(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
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
  auto oversample = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::Oversample, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  auto expoFM = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::ExpoFM, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, expoFM));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, expoFM));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } });

  grid->Add(0, 2, plugGUI->StoreComponent<FBAutoSizeLabel>("AM"));
  grid->Add(1, 2, plugGUI->StoreComponent<FBAutoSizeLabel>("FM"));
  grid->MarkSection({ { 0, 2 }, { 2, 1 } });

  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    grid->Add(0, 3 + i * 3, 2, 1, plugGUI->StoreComponent<FBAutoSizeMultiLineLabel>(FFOsciModFormatSlotVertical(i), -2));
    auto amMode = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::AMMode, i } });
    grid->Add(0, 3 + i * 3 + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, amMode));
    auto amMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::AMMix, i } });
    grid->Add(0, 3 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amMix, Slider::SliderStyle::LinearHorizontal));
    auto fmOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::FMOn, i } });
    grid->Add(1, 3 + i * 3 + 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, fmOn));
    auto fmIndex = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::FMIndex, i } });
    grid->Add(1, 3 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex, Slider::SliderStyle::LinearHorizontal));
    grid->MarkSection({ { 0, 3 + i * 3 }, { 2, 3 } });
  }
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeOsciModGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI);
  tabs->AddModuleTab({ (int)FFModuleType::OsciMod, 0 }, MakeOsciModSectionAll(plugGUI));
  return tabs;
}