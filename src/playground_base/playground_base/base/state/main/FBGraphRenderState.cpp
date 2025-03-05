#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <cassert>

static FBNoteEvent
MakeNoteC4On()
{
  FBNoteEvent result;
  result.pos = 0;
  result.on = true;
  result.velo = 1.0f;
  result.note.id = 0;
  result.note.key = 60;
  result.note.channel = 0;
  return result;
}

FBGraphRenderState::
~FBGraphRenderState() {}

FBGraphRenderState::
FBGraphRenderState(FBPlugGUI const* plugGUI):
_plugGUI(plugGUI),
_input(std::make_unique<FBPlugInputBlock>()),
_moduleState(std::make_unique<FBModuleProcState>()),
_procState(std::make_unique<FBProcStateContainer>(*plugGUI->HostContext()->Topo())),
_primaryVoiceManager(std::make_unique<FBVoiceManager>(_procState.get())),
_exchangeVoiceManager(std::make_unique<FBVoiceManager>(_procState.get()))
{
  _input->note = &_notes;
  _input->audio = &_audio;

  auto hostContext = plugGUI->HostContext();
  for (int i = 0; i < hostContext->Topo()->audio.params.size(); i++)
    _procState->InitProcessing(i, hostContext->GetAudioParamNormalized(i));

  _moduleState->moduleSlot = -1;
  _moduleState->input = _input.get();
  _moduleState->topo = hostContext->Topo();
  _moduleState->procRaw = _procState->Raw();
  _moduleState->exchangeToGUIRaw = nullptr;
  _moduleState->exchangeFromGUIRaw = nullptr;
  _moduleState->outputParamsNormalized = nullptr;
}

FBPlugGUI const* 
FBGraphRenderState::PlugGUI() const
{
  return _plugGUI;
}

FBModuleProcState* 
FBGraphRenderState::ModuleProcState()
{
  return _moduleState.get();
}

FBModuleProcState const* 
FBGraphRenderState::ModuleProcState() const
{
  return _moduleState.get();
}

FBExchangeStateContainer const*
FBGraphRenderState::ExchangeContainer() const
{
  return _plugGUI->HostContext()->ExchangeState();
}

void
FBGraphRenderState::PrepareForRenderExchangeVoice(int voice)
{
  _moduleState->voice = &_exchangeVoiceManager->Voices()[voice];
}

void
FBGraphRenderState::PrepareForRenderExchange()
{
  _moduleState->voice = nullptr;
  _input->voiceManager = nullptr;
  _procState->InitProcessing(*ExchangeContainer());
  _moduleState->exchangeFromGUIRaw = ExchangeContainer()->Raw();
  _exchangeVoiceManager->InitFromExchange(ExchangeContainer()->Voices());
}

void
FBGraphRenderState::PrepareForRenderPrimary(float sampleRate, float bpm)
{
  _input->bpm = bpm;
  _input->sampleRate = sampleRate;
  _input->voiceManager = nullptr;
  _moduleState->voice = nullptr;
  _procState->InitProcessing(_plugGUI->HostContext());
}

void
FBGraphRenderState::PrepareForRenderPrimaryVoice()
{
  if (_primaryVoiceManager->VoiceCount() > 0)
  {
    _primaryVoiceManager->Return(0);
    _primaryVoiceManager->ResetReturnedVoices();
  }
  _primaryVoiceManager->Lease(MakeNoteC4On());
  _input->voiceManager = _primaryVoiceManager.get();
  _moduleState->voice = &_primaryVoiceManager->Voices()[0];
}

// TODO how much of this we still need?
bool 
FBGraphRenderState::GUIParamBool(
  FBParamTopoIndices const& indices) const
{
  auto param = ModuleProcState()->topo->gui.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(normalized);
}

float
FBGraphRenderState::GUIParamLinear(
  FBParamTopoIndices const& indices) const
{
  auto param = ModuleProcState()->topo->gui.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Linear().NormalizedToPlainFast(normalized);
}

int 
FBGraphRenderState::GUIParamLinearTimeSamples(
  FBParamTopoIndices const& indices, float sampleRate) const
{
  auto param = ModuleProcState()->topo->gui.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Linear().NormalizedTimeToSamplesFast(normalized, sampleRate);
}

int 
FBGraphRenderState::GUIParamBarsSamples(
  FBParamTopoIndices const& indices, float sampleRate, float bpm) const
{
  auto param = ModuleProcState()->topo->gui.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetGUIParamNormalized(param->runtimeParamIndex);
  return param->static_.Bars().NormalizedBarsToSamplesFast(normalized, sampleRate, bpm);
}

int
FBGraphRenderState::AudioParamNote(
  FBParamTopoIndices const& indices) const
{
  auto param = ModuleProcState()->topo->audio.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Note().NormalizedToPlain(normalized);
}

bool 
FBGraphRenderState::AudioParamBool(
  FBParamTopoIndices const& indices) const
{
  auto param = ModuleProcState()->topo->audio.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Boolean().NormalizedToPlainFast(normalized);
}

float
FBGraphRenderState::AudioParamLinear(
  FBParamTopoIndices const& indices) const
{
  auto param = ModuleProcState()->topo->audio.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Linear().NormalizedToPlainFast(normalized);
}

int
FBGraphRenderState::AudioParamLinearTimeSamples( 
  FBParamTopoIndices const& indices, float sampleRate) const
{
  auto param = ModuleProcState()->topo->audio.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Linear().NormalizedTimeToSamplesFast(normalized, sampleRate);
}

int
FBGraphRenderState::AudioParamLinearFreqSamples(
  FBParamTopoIndices const& indices, float sampleRate) const
{
  auto param = ModuleProcState()->topo->audio.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Linear().NormalizedFreqToSamplesFast(normalized, sampleRate);
}

int
FBGraphRenderState::AudioParamBarsSamples(
  FBParamTopoIndices const& indices, float sampleRate, float bpm) const
{
  auto param = ModuleProcState()->topo->audio.ParamAtTopo(indices);
  float normalized = _plugGUI->HostContext()->GetAudioParamNormalized(param->runtimeParamIndex);
  return param->static_.Bars().NormalizedBarsToSamplesFast(normalized, sampleRate, bpm);
}

bool
FBGraphRenderState::GlobalModuleExchangeStateEqualsPrimary(
  int moduleIndex, int moduleSlot) const
{
  auto context = _plugGUI->HostContext();
  auto topo = _plugGUI->HostContext()->Topo();
  auto paramIndex = topo->audio.paramTopoToRuntime.at({ moduleIndex, moduleSlot, 0, 0 });
  auto runtimeModuleIndex = topo->audio.params[paramIndex].runtimeModuleIndex;
  for (; topo->audio.params[paramIndex].runtimeModuleIndex == runtimeModuleIndex; paramIndex++)
    if (*ExchangeContainer()->Params()[paramIndex].Global() != context->GetAudioParamNormalized(paramIndex))
      return false;
  return true;
}

bool 
FBGraphRenderState::VoiceModuleExchangeStateEqualsPrimary(
  int voice, int moduleIndex, int moduleSlot) const
{
  auto context = _plugGUI->HostContext();
  auto topo = _plugGUI->HostContext()->Topo();
  auto paramIndex = topo->audio.paramTopoToRuntime.at({ moduleIndex, moduleSlot, 0, 0 });
  auto runtimeModuleIndex = topo->audio.params[paramIndex].runtimeModuleIndex;
  for (; topo->audio.params[paramIndex].runtimeModuleIndex == runtimeModuleIndex; paramIndex++)
    if (ExchangeContainer()->Params()[paramIndex].Voice()[voice] != context->GetAudioParamNormalized(paramIndex))
      return false;
  return true;
}