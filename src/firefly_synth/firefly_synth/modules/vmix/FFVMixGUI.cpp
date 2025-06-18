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
      columnSizes.push_back(0);
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, columnSizes);
  for (int e = 0; e < FFEffectCount; e++)
  {
    int row = e / 2;
    int colStart = (e % 2) * (FFOsciCount + 1);
    std::string name = "OSC\U00002192FX " + std::to_string(e + 1);
    grid->Add(row, colStart, plugGUI->StoreComponent<FBAutoSizeLabel>(name));
    for (int o = 0; o < FFOsciCount; o++)
    {
      int route = e * FFOsciCount + o;
      auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToVFX, route } });
      grid->Add(row, colStart + 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
    }
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFVMixOsciToVFXCount / 2 + 2 } });
  return grid;
}

static Component*
MakeVMixSectionVFXToVFX(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  for (int s = 0; s < FFVMixVFXToVFXCount; s++)
  {
    int row = s / (FFVMixVFXToVFXCount / 2);
    int colStart = s % (FFVMixVFXToVFXCount / 2);
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::VFXToVFX, s } });
    grid->Add(row, colStart * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
    grid->Add(row, colStart * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return grid;
}

static Component*
MakeVMixSectionOsciAndVFXToOut(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  columnSizes.push_back(0);
  assert(FFOsciCount == FFEffectCount);
  for (int i = 0; i < FFOsciCount; i++)
    columnSizes.push_back(1);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("OSC\U00002192Out"));
  for (int o = 0; o < FFOsciCount; o++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToOut, o } });
    grid->Add(0, 1 + o, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("FX\U00002192Out"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::VFXToOut, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFOsciCount + 1 } });
  return grid;
}

static Component*
MakeVMixTab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, MakeVMixSectionOsciToVFX(plugGUI));
  grid->Add(0, 1, MakeVMixSectionVFXToVFX(plugGUI));
  grid->Add(0, 2, MakeVMixSectionOsciAndVFXToOut(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeVMixGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI);
  tabs->AddModuleTab(false, { (int)FFModuleType::VMix, 0 }, MakeVMixTab(plugGUI));
  return tabs;
}