#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/vmix/FFVMixGUI.hpp>
#include <firefly_synth/modules/vmix/FFVMixTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeVMixSectionOsciToVFX(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  for (int i = 0; i < FFEffectCount / 2; i++)
  {
    columnSizes.push_back(0);
    for (int j = 0; j < FFOsciCount; j++)
      columnSizes.push_back(1);
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, columnSizes);
  for (int e = 0; e < FFEffectCount; e++)
  {
    int row = e / 2;
    int colStart = (e % 2) * (FFOsciCount + 1);
    std::string name = "Osc->VFX" + std::to_string(e + 1);
    grid->Add(row, colStart, plugGUI->StoreComponent<FBAutoSizeLabel>(name));
    for (int o = 0; o < FFOsciCount; o++)
    {
      int route = e * FFOsciCount + o;
      auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToVFX, route } });
      grid->Add(row, colStart + 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryHorizontalVerticalDrag));
    }
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFVMixOsciToVFXCount / 2 + 2 } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

static Component*
MakeVMixSectionStringOsciToVFX(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  for (int i = 0; i < FFEffectCount / 2; i++)
  {
    columnSizes.push_back(0);
    for (int j = 0; j < FFStringOsciCount; j++)
      columnSizes.push_back(1);
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, columnSizes);
  for (int e = 0; e < FFEffectCount; e++)
  {
    int row = e / 2;
    int colStart = (e % 2) * (FFStringOsciCount + 1);
    std::string name = "String->VFX" + std::to_string(e + 1);
    grid->Add(row, colStart, plugGUI->StoreComponent<FBAutoSizeLabel>(name));
    for (int o = 0; o < FFStringOsciCount; o++)
    {
      int route = e * FFStringOsciCount + o;
      auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::StringOsciToVFX, route } });
      grid->Add(row, colStart + 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryHorizontalVerticalDrag));
    }
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFVMixStringOsciToVFXCount / 2 + 2 } });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

static Component*
MakeVMixTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 1, 1 });
  grid->Add(0, 0, MakeVMixSectionOsciToVFX(plugGUI));
  grid->Add(0, 1, MakeVMixSectionStringOsciToVFX(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeVMixGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI);
  tabs->AddModuleTab({ (int)FFModuleType::VMix, 0 }, MakeVMixTab(plugGUI));
  return tabs;
}