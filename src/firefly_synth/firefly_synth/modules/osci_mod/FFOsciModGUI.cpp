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
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>

using namespace juce;

static Component*
FFMakeOsciModGUISectionMain(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0 });
  auto oversample = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::Oversample, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  auto expoFM = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::ExpoFM, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, expoFM));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, expoFM));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
FFMakeOsciModGUISectionMod(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  columnSizes.push_back(0);
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    columnSizes.push_back(0);
    columnSizes.push_back(0);
    columnSizes.push_back(1);
  }
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("AM"));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("FM"));
  grid->MarkSection({ { 0, 0 }, { 2, 1 } });
  for (int i = 0; i < FFOsciModSlotCount; i++)
  {
    grid->Add(0, 1 + i * 3, 2, 1, plugGUI->StoreComponent<FBAutoSizeMultiLineLabel>(FFOsciModFormatSlotVertical(i), -2));
    auto amMode = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::AMMode, i } });
    grid->Add(0, 1 + i * 3 + 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, amMode));
    auto amMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::AMMix, i } });
    grid->Add(0, 1 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amMix, Slider::SliderStyle::LinearHorizontal));
    auto fmOn = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::FMOn, i } });
    grid->Add(1, 1 + i * 3 + 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, fmOn));
    auto fmIndex = topo->audio.ParamAtTopo({ { (int)FFModuleType::OsciMod, 0 }, { (int)FFOsciModParam::FMIndex, i } });
    grid->Add(1, 1 + i * 3 + 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fmIndex, Slider::SliderStyle::LinearHorizontal));
    grid->MarkSection({ { 0, 1 + i * 3 }, { 2, 3 } });
  }
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

Component*
FFMakeOsciModTab(FBPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 1 });
  grid->Add(0, 0, FFMakeOsciModGUISectionMain(plugGUI));
  grid->Add(0, 1, FFMakeOsciModGUISectionMod(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}