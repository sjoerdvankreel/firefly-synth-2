#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/mix/FFMixGUI.hpp>
#include <firefly_synth/modules/mix/FFGMixGUI.hpp>
#include <firefly_synth/modules/mix/FFGMixTopo.hpp>

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
MakeGMixSectionVoiceToGFX(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 1 });
  for (int e = 0; e < FFEffectCount; e++)
  {
    int row = e / 2;
    int col = e % 2;
    auto mix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::VoiceToGFX, e } });
    grid->Add(row, col * 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mix));
    grid->Add(row, col * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, mix, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return grid;
}

static Component*
MakeGMixSectionVoiceAndVFXToOut(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  std::vector<int> columnSizes = {};
  auto topo = plugGUI->HostContext()->Topo();
  columnSizes.push_back(0);
  for (int i = 0; i < FFEffectCount; i++)
    columnSizes.push_back(1);
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1, 1 }, columnSizes);
  auto voiceMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::VoiceToOut, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceMix));
  grid->Add(0, 1, 1, FFEffectCount, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceMix, Slider::SliderStyle::LinearHorizontal));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("FX\U00002192Out"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto fxMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::GFXToOut, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fxMix, Slider::SliderStyle::LinearHorizontal));
  }
  grid->MarkSection({ { 0, 0 }, { 2, FFEffectCount + 1 } });
  return grid;
}

Component*
FFMakeGMixGUITab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 1 });
  grid->Add(0, 0, MakeGMixSectionVoiceToGFX(plugGUI));
  grid->Add(0, 1, FFMakeMixGUISectionFXToFX(plugGUI, (int)FFModuleType::GMix, (int)FFGMixParam::GFXToGFX));
  grid->Add(0, 2, MakeGMixSectionVoiceAndVFXToOut(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}