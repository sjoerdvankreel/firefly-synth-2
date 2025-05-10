#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFPlugProcessor.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/dsp/host/FBHostDSPContext.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBProcStateContainer.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>

#include <cmath>
#include <numbers>

FFPlugProcessor::
FFPlugProcessor(IFBHostDSPContext* hostContext) :
_topo(hostContext->Topo()),
_sampleRate(hostContext->SampleRate()),
_procState(static_cast<FFProcState*>(hostContext->ProcState()->Raw())),
_exchangeState(static_cast<FFExchangeState*>(hostContext->ExchangeState()->Raw())) {}

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
  for (int n = 0; n < input.note->size(); n++)
    if ((*input.note)[n].on)
    {
      int voice = input.voiceManager->Lease((*input.note)[n]);
      auto state = MakeModuleVoiceState(input, voice);
      _procState->dsp.voice[voice].processor.BeginVoice(state);
    }
}

void
FFPlugProcessor::ProcessPreVoice(
  FBPlugInputBlock const& input)
{
  auto state = MakeModuleState(input);
  for (int s = 0; s < FFGLFOCount; s++)
  {
    state.moduleSlot = s;
    _procState->dsp.global.gLFO[s].processor->Process(state);
  }
}

void
FFPlugProcessor::ProcessPostVoice(
  FBPlugInputBlock const& input, FBPlugOutputBlock& output)
{
  auto& gGilterIn = _procState->dsp.global.gFilter[0].input;
  gGilterIn.Fill(0.0f);
  for (int v = 0; v < FBMaxVoices; v++)
    if (input.voiceManager->IsActive(v))
      gGilterIn.Add(_procState->dsp.voice[v].output);

  auto state = MakeModuleState(input);
  state.moduleSlot = 0;
  state.outputParamsNormalized = &output.outputParamsNormalized;
  _procState->dsp.global.gFilter[0].processor->Process(state);
  for (int s = 1; s < FFGFilterCount; s++)
  {
    state.moduleSlot = s;
    _procState->dsp.global.gFilter[s - 1].output.CopyTo(_procState->dsp.global.gFilter[s].input);
    _procState->dsp.global.gFilter[s].processor->Process(state);
  }

  state.moduleSlot = 0;
  _procState->dsp.global.gFilter[FFGFilterCount - 1].output.CopyTo(_procState->dsp.global.master.input);
  _procState->dsp.global.master.processor->Process(state);
  _procState->dsp.global.master.output.CopyTo(output.audio);
}