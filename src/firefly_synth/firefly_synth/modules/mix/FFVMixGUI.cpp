#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixGUI.hpp>
#include <firefly_synth/modules/mix/FFVMixGUI.hpp>
#include <firefly_synth/modules/mix/FFVMixTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
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
  for (int i = 0; i < FFOsciCount / 2; i++)
  {
    columnSizes.push_back(0);
    for (int j = 0; j < FFEffectCount; j++)
    {
      columnSizes.push_back(0);
      columnSizes.push_back(0);
    }
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  for (int o = 0; o < FFOsciCount; o++)
  {
    int row = o / 2;
    int colStart = (o % 2) * (2 * FFEffectCount + 1);
    std::string name = "Osc" + std::to_string(o + 1) + "\U00002192FX";
    grid->Add(row, colStart, plugGUI->StoreComponent<FBAutoSizeLabel>(name));
    for (int e = 0; e < FFEffectCount; e++)
    {
      int route = o * FFEffectCount + e;
      auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToVFX, route } });
      grid->Add(row, colStart + 1 + e * 2, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(e + 1)));
      grid->Add(row, colStart + 1 + e * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
    }
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFVMixOsciToVFXCount + 2 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
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
  {
    columnSizes.push_back(0);
    columnSizes.push_back(1);
  }
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Osc\U00002192Out"));
  for (int o = 0; o < FFOsciCount; o++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::OsciToOut, o } });
    grid->Add(0, 1 + 2 * o, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(o + 1)));
    grid->Add(0, 1 + 2 * o + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("FX\U00002192Out"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::VFXToOut, e } });
    grid->Add(1, 1 + 2 * e, plugGUI->StoreComponent<FBAutoSizeLabel>(std::to_string(e + 1)));
    grid->Add(1, 1 + 2 * e + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFOsciCount * 2 + 1 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeVMixGUISectionAmpBal(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto amp = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0}, { (int)FFVMixParam::Amp, 0} });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amp));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amp, Slider::SliderStyle::RotaryVerticalDrag));
  auto ampEnvToAmp = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::AmpEnvToAmp, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, ampEnvToAmp));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ampEnvToAmp, Slider::SliderStyle::RotaryVerticalDrag));
  auto bal = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::Bal, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bal));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bal, Slider::SliderStyle::RotaryVerticalDrag));
  auto lfo1ToBal = topo->audio.ParamAtTopo({ { (int)FFModuleType::VMix, 0 }, { (int)FFVMixParam::LFO1ToBal, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo1ToBal));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo1ToBal, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

Component*
FFMakeVMixGUITab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 1, 0, 0, 0 });
  grid->Add(0, 0, MakeVMixSectionOsciAndVFXToOut(plugGUI));
  grid->Add(0, 1, MakeVMixSectionOsciToVFX(plugGUI));
  grid->Add(0, 2, FFMakeMixGUISectionFXToFX(plugGUI, (int)FFModuleType::VMix, (int)FFVMixParam::VFXToVFX));
  grid->Add(0, 3, MakeVMixGUISectionAmpBal(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}