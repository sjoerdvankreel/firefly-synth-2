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
#include <firefly_base/gui/components/FBCardComponent.hpp>
#include <firefly_base/gui/components/FBMarginComponent.hpp>
#include <firefly_base/gui/components/FBThemingComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

static Component*
FFMakeOsciModGUISectionMain(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, std::vector<int> { 1, 0 });
  auto oversample = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::Oversample, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  auto expoFM = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::ExpoFM, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, expoFM));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, expoFM));
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

static Component*
FFMakeOsciModGUISectionMod(FBPlugGUI* plugGUI, int block, int slotStart)
{
  std::vector<int> columnSizes = {};
  for (int i = 0; i <= block; i++)
  {
    columnSizes.push_back(0);
    columnSizes.push_back(0);
    columnSizes.push_back(0);
    columnSizes.push_back(0);
  }
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1, 1 }, columnSizes);
  for (int i = 0; i <= block; i++)
  {
    int slot = slotStart + i;
    grid->Add(0, i * 4, 2, 1, plugGUI->StoreComponent<FBAutoSizeMultiLineLabel>(plugGUI, FFOsciModFormatSlotVertical(slot), -2));
    auto amMode = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::AMMode, slot } });
    grid->Add(0, i * 4 + 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amMode));
    grid->Add(0, i * 4 + 2, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, amMode));
    auto amMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::AMMix, slot } });
    grid->Add(0, i * 4 + 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amMix, Slider::SliderStyle::RotaryVerticalDrag));
    auto fmOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::FMOn, slot } });
    grid->Add(1, i * 4 + 1, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fmOn));
    grid->Add(1, i * 4 + 2, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, fmOn));
    auto fmIndex = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::FMIndex, slot } });
    grid->Add(1, i * 4 + 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex, Slider::SliderStyle::RotaryVerticalDrag));
  }
  return plugGUI->StoreComponent<FBCardComponent>(plugGUI, grid);
}

Component*
FFMakeOsciModTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(plugGUI, true, std::vector<int> { 1 }, std::vector<int> { 1, 0, 0, 0 });
  grid->Add(0, 0, FFMakeOsciModGUISectionMain(plugGUI));
  grid->Add(0, 1, FFMakeOsciModGUISectionMod(plugGUI, 0, 0));
  grid->Add(0, 2, FFMakeOsciModGUISectionMod(plugGUI, 1, 1));
  grid->Add(0, 3, FFMakeOsciModGUISectionMod(plugGUI, 2, 3));
  return plugGUI->StoreComponent<FBModuleComponent>(plugGUI->HostContext()->Topo(), (int)FFModuleType::OsciMod, 0, grid);
}