#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFModulate.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleGUI.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>

#include <firefly_base/base/shared/FBLogging.hpp>
#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBToggleButton.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBFillerComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/gui/components/FBModuleComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

FFVoiceModuleParamListener::
~FFVoiceModuleParamListener()
{
  _plugGUI->RemoveParamListener(this);
}

FFVoiceModuleParamListener::
FFVoiceModuleParamListener(FBPlugGUI* plugGUI):
_plugGUI(plugGUI)
{
  _plugGUI->AddParamListener(this);
}

void 
FFVoiceModuleParamListener::AudioParamChanged(
  int index, double /*normalized*/, bool /*changedFromUI*/)
{
  auto const& indices = _plugGUI->HostContext()->Topo()->audio.params[index].topoIndices;
  if (indices.module.index != (int)FFModuleType::VoiceModule)
    return;
  if (indices.param.index == (int)FFVoiceModuleParam::Env5ToCoarse)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Coarse, 0 } });
  if (indices.param.index == (int)FFVoiceModuleParam::LFO5ToFine)
    _plugGUI->RepaintSlidersForAudioParam({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Fine, 0 } });
}

bool
FFVoiceModuleAdjustParamModulationGUIBounds(
  FBHostGUIContext const* ctx, int index, float& currentMinNorm, float& currentMaxNorm)
{
  auto const& rtParam = ctx->Topo()->audio.params[index];
  int staticIndex = ctx->Topo()->modules[rtParam.runtimeModuleIndex].topoIndices.index;
  if (staticIndex != (int)FFModuleType::VoiceModule)
    return false;

  if (rtParam.topoIndices.param.index == (int)FFVoiceModuleParam::Coarse)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Env5ToCoarse, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::UPStack, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  if (rtParam.topoIndices.param.index == (int)FFVoiceModuleParam::Fine)
  {
    double modAmount = ctx->GetAudioParamNormalized({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::LFO5ToFine, 0 } });
    FFApplyGUIModulationBounds(FFModulationOpType::BPStack, 0.0f, 1.0f, (float)modAmount, currentMinNorm, currentMaxNorm);
    return true;
  }

  return false;
}

static Component*
MakeVoiceModuleSectionPitch(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto coarse = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Coarse, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, coarse));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, coarse, Slider::SliderStyle::RotaryVerticalDrag));
  auto env5ToCoarse = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Env5ToCoarse, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, env5ToCoarse));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, env5ToCoarse, Slider::SliderStyle::RotaryVerticalDrag));
  auto fine = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::Fine, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, fine));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, fine, Slider::SliderStyle::RotaryVerticalDrag));
  auto lfo5ToFine = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::LFO5ToFine, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, lfo5ToFine));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, lfo5ToFine, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 4 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(false, grid);
}

static Component*
MakeVoiceModuleSectionPorta(FBPlugGUI* plugGUI)
{
  auto topo = plugGUI->HostContext()->Topo();
  std::vector<int> columnSizes = { 0, 0, 0, 0, 0, 0 };
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1, 1 }, columnSizes);
  auto type = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaType, 0 } });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto sync = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaSync, 0 } });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sync));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, sync));
  auto mode = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaMode, 0 } });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, mode));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, mode));
  auto time = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaTime, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, time));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, time, Slider::SliderStyle::LinearHorizontal));
  auto bars = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaBars, 0 } });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, bars));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, bars));
  auto ampAttack = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaSectionAmpAttack, 0 } });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, ampAttack));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ampAttack, Slider::SliderStyle::RotaryVerticalDrag));
  auto ampRelease = topo->audio.ParamAtTopo({ { (int)FFModuleType::VoiceModule, 0 }, { (int)FFVoiceModuleParam::PortaSectionAmpRelease, 0 } });
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, ampRelease));
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, ampRelease, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ { 0, 0 }, { 2, 6 } });
  return plugGUI->StoreComponent<FBSubSectionComponent>(true, grid);
}

static Component*
MakeVoiceModuleTab(FBPlugGUI* plugGUI)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(true, std::vector<int> { 1 }, std::vector<int> { 0, 0 });
  grid->Add(0, 0, MakeVoiceModuleSectionPitch(plugGUI));
  grid->Add(0, 1, MakeVoiceModuleSectionPorta(plugGUI));
  auto section = plugGUI->StoreComponent<FBSectionComponent>(grid);
  return plugGUI->StoreComponent<FBModuleComponent>((int)FFModuleType::VoiceModule, 0, section);
}

Component*
FFMakeVoiceModuleGUI(FBPlugGUI* plugGUI)
{
  FB_LOG_ENTRY_EXIT();
  auto tabs = plugGUI->StoreComponent<FBAutoSizeTabComponent>();
  auto name = plugGUI->HostContext()->Topo()->static_->modules[(int)FFModuleType::VoiceModule].name;
  tabs->addTab(name, {}, MakeVoiceModuleTab(plugGUI), false);
  return tabs;
}