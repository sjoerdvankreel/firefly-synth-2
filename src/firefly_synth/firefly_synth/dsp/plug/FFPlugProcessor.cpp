#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/plug/FFPlugProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/master/FFMasterProcessor.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>
 
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/dsp/host/FBHostDSPContext.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>
#include <firefly_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <cmath>
#include <numbers>

FFPlugProcessor::
FFPlugProcessor(IFBHostDSPContext* hostContext) :
_topo(hostContext->Topo()),
_sampleRate(hostContext->SampleRate()),
_procState(static_cast<FFProcState*>(hostContext->ProcState()->Raw())),
_exchangeState(static_cast<FFExchangeState*>(hostContext->ExchangeState()->Raw()))
{
  for (int i = 0; i < FFEffectCount; i++)
    _procState->dsp.global.gEffect[i].processor->InitializeBuffers(false, _sampleRate);
  for (int v = 0; v < FBMaxVoices; v++)
  {
    for (int i = 0; i < FFEffectCount; i++)
      _procState->dsp.voice[v].vEffect[i].processor->InitializeBuffers(false, _sampleRate);
    for (int i = 0; i < FFStringOsciCount; i++)
      _procState->dsp.voice[v].stringOsci[i].processor->InitializeBuffers(_sampleRate);
  }
}

FBModuleProcState
FFPlugProcessor::MakeModuleState(
  FBPlugInputBlock const& input) const
{
  FBModuleProcState result = {};
  result.topo = _topo;
  result.input = &input;
  result.procRaw = _procState;
  result.exchangeFromGUIRaw = nullptr;
  result.exchangeToGUIRaw = _exchangeState;
  result.renderType = FBRenderType::Audio;
  return result;
}

FBModuleProcState
FFPlugProcessor::MakeModuleVoiceState(
  FBPlugInputBlock const& input, int voice) const
{
  auto result = MakeModuleState(input);
  result.voice = &result.input->voiceManager->Voices()[voice];
  return result;
}

void
FFPlugProcessor::ProcessVoice(
  FBPlugInputBlock const& input, int voice)
{
  auto state = MakeModuleVoiceState(input, voice);
  if(_procState->dsp.voice[voice].processor.Process(state))
    input.voiceManager->Return(voice);
}

void 
FFPlugProcessor::LeaseVoices(
  FBPlugInputBlock const& input)
{
  for (int n = 0; n < input.noteEvents->size(); n++)
    if ((*input.noteEvents)[n].on)
    {
      int voice = input.voiceManager->Lease((*input.noteEvents)[n]);
      auto state = MakeModuleVoiceState(input, voice);
      _procState->dsp.voice[voice].processor.BeginVoice(state);
    }
}

void
FFPlugProcessor::ProcessPostVoice(
  FBPlugInputBlock const& input, FBPlugOutputBlock& output)
{
  auto& gEffectIn = _procState->dsp.global.gEffect[0].input;
  gEffectIn.Fill(0.0f);
  for (int v = 0; v < FBMaxVoices; v++)
    if (input.voiceManager->IsActive(v))
      gEffectIn.Add(_procState->dsp.voice[v].output);

  auto state = MakeModuleState(input);
  state.moduleSlot = 0;
  state.outputParamsNormalized = &output.outputParamsNormalized;
  _procState->dsp.global.gEffect[0].processor->BeginVoiceOrBlock<true>(false, -1, -1, state);
  _procState->dsp.global.gEffect[0].processor->Process<true>(state);
  for (int s = 1; s < FFEffectCount; s++)
  {
    state.moduleSlot = s;
    _procState->dsp.global.gEffect[s - 1].output.CopyTo(_procState->dsp.global.gEffect[s].input);
    _procState->dsp.global.gEffect[s].processor->BeginVoiceOrBlock<true>(false, -1, -1, state);
    _procState->dsp.global.gEffect[s].processor->Process<true>(state);
  }

  state.moduleSlot = 0;
  _procState->dsp.global.gEffect[FFEffectCount - 1].output.CopyTo(_procState->dsp.global.master.input);
  _procState->dsp.global.master.processor->Process(state);
  _procState->dsp.global.master.output.CopyTo(output.audio);
  _procState->dsp.global.output.processor->Process(state);
}