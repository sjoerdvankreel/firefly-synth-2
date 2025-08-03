#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/effect/FFEffectGUI.hpp>
#include <firefly_synth/modules/effect/FFEffectTopo.hpp>
#include <firefly_synth/modules/gui_settings/FFGUISettingsTopo.hpp>

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
#include <firefly_base/gui/components/FBParamsDependentComponent.hpp>

using namespace juce;

static Component*
MakeEffectSectionMain(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();

  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 1 });
  auto on = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::On, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto oversample = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Oversample, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, oversample));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, oversample));
  auto trackingKey = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::TrackingKey, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, trackingKey));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, trackingKey, Slider::SliderStyle::RotaryVerticalDrag));
  auto lastKeySmoothTime = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::LastKeySmoothTime, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lastKeySmoothTime));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lastKeySmoothTime, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return grid;
}

static Component*
MakeEffectSectionBlock(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot, int block)
{
  FB_LOG_ENTRY_EXIT();

  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0, 0, 0, 0, 0 });
  auto kind = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::Kind, block } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, kind, std::string(1, static_cast<char>('A' + block))));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, kind));
  
  auto combKeyTrk = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombKeyTrk, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combKeyTrk));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combKeyTrk, Slider::SliderStyle::LinearHorizontal));
  auto combResPlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombResPlus, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResPlus));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combResMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombResMin, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combResMin));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combResMin, Slider::SliderStyle::RotaryVerticalDrag));
  auto combFreqPlus = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqPlus, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqPlus));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqPlus, Slider::SliderStyle::RotaryVerticalDrag));
  auto combFreqMin = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::CombFreqMin, block } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, combFreqMin));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, combFreqMin, Slider::SliderStyle::RotaryVerticalDrag));

  auto stVarMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarMode, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, stVarMode));
  auto stVarRes = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarRes, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarRes));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarRes, Slider::SliderStyle::RotaryVerticalDrag));
  auto stVarFreq = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarFreq, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarFreq));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarFreq, Slider::SliderStyle::RotaryVerticalDrag));
  auto stVarGain = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarGain, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarGain));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarGain, Slider::SliderStyle::RotaryVerticalDrag));
  auto stVarKeyTrak = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::StVarKeyTrak, block } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, stVarKeyTrak));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, stVarKeyTrak, Slider::SliderStyle::RotaryVerticalDrag));

  auto clipMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::ClipMode, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, clipMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, clipMode));
  auto foldMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::FoldMode, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, foldMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, foldMode));
  auto skewMode = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::SkewMode, block } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, skewMode));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, skewMode));
  auto distMix = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistMix, block } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distMix));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distMix, Slider::SliderStyle::RotaryVerticalDrag));
  auto distDrive = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistDrive, block } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distDrive));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distDrive, Slider::SliderStyle::RotaryVerticalDrag));
  auto distBias = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistBias, block } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distBias));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distBias, Slider::SliderStyle::RotaryVerticalDrag));
  auto distAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::DistAmt, block } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, distAmt));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, distAmt, Slider::SliderStyle::RotaryVerticalDrag));

  auto envAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::EnvAmt, block } });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, envAmt));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, envAmt, Slider::SliderStyle::RotaryVerticalDrag));
  auto lfoAmt = topo->audio.ParamAtTopo({ { (int)moduleType, moduleSlot }, { (int)FFEffectParam::LFOAmt, block } });
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfoAmt));
  grid->Add(1, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfoAmt, Slider::SliderStyle::RotaryVerticalDrag));

  grid->MarkSection({ { 0, 0 }, { 2, 8 } });
  return grid;
}

static Component*
MakeEffectTab(FBPlugGUI* plugGUI, FFModuleType moduleType, int moduleSlot)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  columnSizes.push_back(1);
  for (int i = 0; i < FFEffectBlockCount; i++)
    columnSizes.push_back(0);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, columnSizes);
  grid->Add(0, 0, MakeEffectSectionMain(plugGUI, moduleType, moduleSlot));
  for(int i = 0; i < FFEffectBlockCount; i++)
    grid->Add(0, 1 + i, MakeEffectSectionBlock(plugGUI, moduleType, moduleSlot, i));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeEffectGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto tabParam = topo->gui.ParamAtTopo({ { (int)FFModuleType::GUISettings, 0 }, { (int)FFGUISettingsGUIParam::FXSelectedTab, 0 } });
  auto tabs = plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, tabParam);
  for (int i = 0; i < FFEffectCount; i++)
    tabs->AddModuleTab(i != 0, { (int)FFModuleType::VEffect, i }, MakeEffectTab(plugGUI, FFModuleType::VEffect, i));
  for (int i = 0; i < FFEffectCount; i++)
    tabs->AddModuleTab(i != 0, { (int)FFModuleType::GEffect, i }, MakeEffectTab(plugGUI, FFModuleType::GEffect, i));
  tabs->ActivateStoredSelectedTab();
  return tabs;
}