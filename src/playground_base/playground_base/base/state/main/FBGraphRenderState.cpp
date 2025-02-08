#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/main/FBScalarStateContainer.hpp>

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
_scalarState(std::make_unique<FBScalarStateContainer>(*plugGUI->HostContext()->Topo())),
_primaryVoiceManager(std::make_unique<FBVoiceManager>(_procState.get())),
_exchangeVoiceManager(std::make_unique<FBVoiceManager>(_procState.get()))
{
  _input->note = &_notes;
  _input->audio = &_audio;

  auto hostContext = plugGUI->HostContext();
  for (int i = 0; i < hostContext->Topo()->params.size(); i++)
    _procState->InitProcessing(i, hostContext->GetParamNormalized(i));

  _moduleState->moduleSlot = -1;
  _moduleState->sampleRate = -1;
  _moduleState->input = _input.get();
  _moduleState->exchangeRaw = nullptr;
  _moduleState->topo = hostContext->Topo();
  _moduleState->procRaw = _procState->Raw();
  _moduleState->outputParamsNormalized = nullptr;
}

FBModuleProcState* 
FBGraphRenderState::ModuleProcState()
{
  return _moduleState.get();
}

FBScalarStateContainer const*
FBGraphRenderState::ScalarContainer() const
{
  return _scalarState.get();
}

FBExchangeStateContainer const*
FBGraphRenderState::ExchangeContainer() const
{
  return _plugGUI->HostContext()->ExchangeState();
}

void
FBGraphRenderState::PrimaryParamChanged(int index, float normalized)
{
  *_scalarState->Params()[index] = normalized;
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
FBGraphRenderState::PrepareForRenderPrimary()
{
  _moduleState->voice = nullptr;
  _input->voiceManager = nullptr;
  _procState->InitProcessing(*_scalarState);
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
  auto topo = _plugGUI->HostContext()->Topo();
  auto paramIndex = topo->paramTopoToRuntime.at({ moduleIndex, moduleSlot, 0, 0 });
  auto runtimeModuleIndex = topo->params[paramIndex].runtimeModuleIndex;
  for (; topo->params[paramIndex].runtimeModuleIndex == runtimeModuleIndex; paramIndex++)
    if (*ExchangeContainer()->Params()[paramIndex].Global() != *_scalarState->Params()[paramIndex])
      return false;
  return true;
}

bool 
FBGraphRenderState::VoiceModuleExchangeStateEqualsPrimary(
  int voice, int moduleIndex, int moduleSlot) const
{
  auto topo = _plugGUI->HostContext()->Topo();
  auto paramIndex = topo->paramTopoToRuntime.at({ moduleIndex, moduleSlot, 0, 0 });
  auto runtimeModuleIndex = topo->params[paramIndex].runtimeModuleIndex;
  for (; topo->params[paramIndex].runtimeModuleIndex == runtimeModuleIndex; paramIndex++)
    if (ExchangeContainer()->Params()[paramIndex].Voice()[voice] != *_scalarState->Params()[paramIndex])
      return false;
  return true;
}