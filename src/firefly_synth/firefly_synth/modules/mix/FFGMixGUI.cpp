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
MakeGMixSectionVoiceToGFXAndExtAudioToGFX(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 1, 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Voice Mix\U00002192GFX"));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Ext Audio\U00002192GFX"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto voiceToGFX = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::VoiceToGFX, e } });
    grid->Add(0, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceToGFX, Slider::SliderStyle::LinearHorizontal));
    auto extAudioToGFX = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::ExtAudioToGFX, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, extAudioToGFX, Slider::SliderStyle::LinearHorizontal));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 5 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGMixSectionGFXToOut(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  for (int e = 0; e < FFEffectCount; e++)
  {
    FB_ASSERT(FFEffectCount % 2 == 0);
    int r = e / (FFEffectCount / 2);
    int c = e % (FFEffectCount / 2);
    grid->Add(r, c * 2 + 0, plugGUI->StoreComponent<FBAutoSizeLabel>("GFX " + std::to_string(e + 1) + "\U00002192Out"));
    auto gfxToOut = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::GFXToOut, e } });
    grid->Add(r, c * 2 + 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gfxToOut, Slider::SliderStyle::RotaryVerticalDrag));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGMixGUISectionVoiceExtAudio(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0 });
  auto voiceMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::VoiceToOut, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceMix));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceMix, Slider::SliderStyle::RotaryVerticalDrag));
  auto extAudioMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::ExtAudioToOut, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, extAudioMix));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, extAudioMix, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

static Component*
MakeGMixGUISectionAmpBal(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 0, 0 });
  auto amp = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0}, { (int)FFGMixParam::Amp, 0}});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amp));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amp, Slider::SliderStyle::LinearHorizontal));
  auto lfo5ToAmp = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0}, { (int)FFGMixParam::LFO5ToAmp, 0} });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo5ToAmp));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo5ToAmp, Slider::SliderStyle::RotaryVerticalDrag));
  auto bal = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::Bal, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bal));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bal, Slider::SliderStyle::LinearHorizontal));
  auto lfo6ToBal = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::LFO6ToBal, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo6ToBal));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo6ToBal, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(grid);
}

Component*
FFMakeGMixGUITab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 4, 4, 0, 0, 3 });
  grid->Add(0, 0, FFMakeMixGUISectionFXToFX(plugGUI, (int)FFModuleType::GMix, (int)FFGMixParam::GFXToGFX, Slider::SliderStyle::LinearHorizontal));
  grid->Add(0, 1, MakeGMixSectionVoiceToGFXAndExtAudioToGFX(plugGUI));
  grid->Add(0, 2, MakeGMixSectionGFXToOut(plugGUI));
  grid->Add(0, 3, MakeGMixGUISectionVoiceExtAudio(plugGUI));
  grid->Add(0, 4, MakeGMixGUISectionAmpBal(plugGUI));
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}