#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
#include <firefly_synth/modules/mix/FFMixGUI.hpp>
#include <firefly_synth/modules/mix/FFGMixGUI.hpp>
#include <firefly_synth/modules/mix/FFGMixTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FFGMixParamListener::
~FFGMixParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFGMixParamListener::
FFGMixParamListener(FBPlugGUI* plugGUI):
_plugGUI(plugGUI)
{
  _plugGUI->AddParamListener(this);
}

void 
FFGMixParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool /*changedFromUI*/)
{
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  if (indices.module.index != (int)FFModuleType::GMix)
    return;
  if (indices.param.index == (int)FFGMixParam::LFO5ToAmp)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::Amp, 0 } });
  if (indices.param.index == (int)FFGMixParam::LFO6ToBal)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::Bal, 0 } });
}

bool
FFGMixAdjustParamModulationGUIBounds(
  FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm)
{
  auto const& rtParam = ctx->Topo()->audio.params[index];
  int staticIndex = ctx->Topo()->modules[rtParam.runtimeModuleIndex].topoIndices.index;
  if (staticIndex != (int)FFModuleType::GMix)
    return false;

  if (rtParam.topoIndices.param.index == (int)FFGMixParam::Amp)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::LFO5ToAmp, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::UPMul, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  if (rtParam.topoIndices.param.index == (int)FFGMixParam::Bal)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::LFO6ToBal, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::BPStack, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  return false;
}

static Component*
MakeGMixSectionVoiceToGFXAndExtAudioToGFX(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 1, 1, 1, 1 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("VMix\U00002192GFX"));
  grid->Add(1, 0, plugGUI->StoreComponent<FBAutoSizeLabel>("Ext Audio\U00002192GFX"));
  for (int e = 0; e < FFEffectCount; e++)
  {
    auto voiceToGFX = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::VoiceToGFX, e } });
    grid->Add(0, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceToGFX, Slider::SliderStyle::LinearHorizontal));
    auto extAudioToGFX = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::ExtAudioToGFX, e } });
    grid->Add(1, 1 + e, plugGUI->StoreComponent<FBParamSlider>(plugGUI, extAudioToGFX, Slider::SliderStyle::LinearHorizontal));
  }
  grid->MarkSection({ { 0, 0 }, { 2, 5 } }, FBGridSectionMark::Alternate);
  return grid;
}

static Component*
MakeGMixSectionGFXToOut(FBPlugGUI* plugGUI)
{
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
  return grid;
}

static Component*
MakeGMixGUISectionVoiceAndExtAudioToOut(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0 });
  auto voiceMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::VoiceToOut, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, voiceMix));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, voiceMix, Slider::SliderStyle::RotaryVerticalDrag));
  auto extAudioMix = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::ExtAudioToOut, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, extAudioMix));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, extAudioMix, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 2 } });
  return grid;
}

static Component*
MakeGMixGUISectionAmpBal(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, std::vector<int> { 0, 0, 0, 0 });
  auto amp = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0}, { (int)FFGMixParam::Amp, 0}});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, amp));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, amp, Slider::SliderStyle::RotaryVerticalDrag));
  auto lfo5ToAmp = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0}, { (int)FFGMixParam::LFO5ToAmp, 0} });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo5ToAmp));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo5ToAmp, Slider::SliderStyle::RotaryVerticalDrag));
  auto bal = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::Bal, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bal));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, bal, Slider::SliderStyle::RotaryVerticalDrag));
  auto lfo6ToBal = topo->audio.ParamAtTopo({ { (int)FFModuleType::GMix, 0 }, { (int)FFGMixParam::LFO6ToBal, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo6ToBal));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo6ToBal, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } }, FBGridSectionMark::Alternate);
  return grid;
}

Component*
FFMakeGMixGUITab(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 1, 1, 0, 0, 0 });
  grid->Add(0, 0, MakeGMixSectionVoiceToGFXAndExtAudioToGFX(plugGUI));
  grid->Add(0, 1, FFMakeMixGUISectionFXToFX(plugGUI, (int)FFModuleType::GMix, (int)FFGMixParam::GFXToGFX));
  grid->Add(0, 2, MakeGMixSectionGFXToOut(plugGUI));
  grid->Add(0, 3, MakeGMixGUISectionVoiceAndExtAudioToOut(plugGUI));
  grid->Add(0, 4, MakeGMixGUISectionAmpBal(plugGUI));
  auto section = plugGUI->StoreComponent<FBSectionComponent>(grid);
  return plugGUI->StoreComponent<FBModuleComponent>((int)FFModuleType::GMix, 0, section);
}