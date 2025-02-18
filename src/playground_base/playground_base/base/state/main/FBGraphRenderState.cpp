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
  _moduleState->exchangeRaw = nullptr;
  _moduleState->topo = hostContext->Topo();
  _moduleState->procRaw = _procState->Raw();
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
FBGraphRenderState::PrepareForRenderExchange()
{
  _moduleState->voice = nullptr;
  _input->voiceManager = nullptr;
  _procState->InitProcessing(*ExchangeContainer());
  _exchangeVoiceManager->InitFromExchange(ExchangeContainer()->Voices());
}

void
FBGraphRenderState::PrepareForRenderExchangeVoice(int voice)
{
  _moduleState->voice = &_exchangeVoiceManager->Voices()[voice];
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