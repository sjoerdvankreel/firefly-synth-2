#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/plug/FFPlugProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
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
_sampleRate(hostContext->SampleRate()),
_topo(hostContext->Topo()),
_procState(static_cast<FFProcState*>(hostContext->ProcState()->Raw())),
_exchangeState(static_cast<FFExchangeState*>(hostContext->ExchangeState()->Raw()))
{
  _procState->dsp.global.gMatrix.processor->InitializeBuffers(_topo);
  for (int i = 0; i < FFEffectCount; i++)
    _procState->dsp.global.gEffect[i].processor->InitializeBuffers(false, _sampleRate);
  for (int v = 0; v < FBMaxVoices; v++)
  {
    _procState->dsp.voice[v].vMatrix.processor->InitializeBuffers(_topo);
    for (int i = 0; i < FFEffectCount; i++)
      _procState->dsp.voice[v].vEffect[i].processor->InitializeBuffers(false, _sampleRate);
    for (int i = 0; i < FFOsciCount; i++)
      _procState->dsp.voice[v].osci[i].processor->InitializeBuffers(false, _sampleRate);
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
FFPlugProcessor::ProcessPreVoice(FBPlugInputBlock const& input)
{
  auto state = MakeModuleState(input);
  auto& globalDSP = _procState->dsp.global;
  state.moduleSlot = 0;
  globalDSP.gMatrix.processor->BeginVoiceOrBlock(state);
  globalDSP.gMatrix.processor->BeginModulationBlock(state);
  for (int i = 0; i < FFLFOAndEnvCount; i++)
  {
    state.moduleSlot = i;
    globalDSP.gLFO[i].processor->BeginVoiceOrBlock<true>(false, -1, -1, nullptr, state);
    globalDSP.gLFO[i].processor->Process<true>(state);
    state.moduleSlot = 0;
    globalDSP.gMatrix.processor->ApplyModulation(state, { (int)FFModuleType::GLFO, i });

    // We can get away with this because FBHostProcessor does LeaseVoices() first.
    for (int v = 0; v < FBMaxVoices; v++)
      if (input.voiceManager->IsActive(v))
      {
        state.voice = &state.input->voiceManager->Voices()[v];
        _procState->dsp.voice[v].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::GLFO, i });
      }
  }
}

void
FFPlugProcessor::ProcessPostVoice(
  FBPlugInputBlock const& input, FBPlugOutputBlock& output)
{
  auto state = MakeModuleState(input);
  auto& globalDSP = _procState->dsp.global;
  auto const& gMix = _procState->param.global.gMix[0];
  auto const& balNorm = gMix.acc.bal[0].Global();
  auto const& gainNorm = gMix.acc.gain[0].Global();
  auto& moduleTopo = state.topo->static_->modules[(int)FFModuleType::GMix];

  FBSArray2<float, FBFixedBlockSamples, 2> voiceMixdown = {};
  voiceMixdown.Fill(0.0f);
  for (int v = 0; v < FBMaxVoices; v++)
    if (input.voiceManager->IsActive(v))
      voiceMixdown.Add(_procState->dsp.voice[v].output);

  for (int i = 0; i < FFEffectCount; i++)
  {
    state.moduleSlot = i;
    globalDSP.gEffect[i].input.Fill(0.0f);
    auto const& voiceToGFXNorm = gMix.acc.voiceToGFX[i].Global().CV();
    globalDSP.gEffect[i].input.AddMul(voiceMixdown, voiceToGFXNorm);
    for (int r = 0; r < FFMixFXToFXCount; r++)
      if (FFMixFXToFXGetTargetSlot(r) == i)
      {
        int source = FFMixFXToFXGetSourceSlot(r);
        auto const& gfxToGFXNorm = gMix.acc.GFXToGFX[r].Global().CV();
        globalDSP.gEffect[i].input.AddMul(globalDSP.gEffect[source].output, gfxToGFXNorm);
      }
    globalDSP.gEffect[i].processor->BeginVoiceOrBlock<true>(false, -1, -1, state);
    globalDSP.gEffect[i].processor->Process<true>(state);
  }

  output.audio.Fill(0.0f);
  auto const& voiceToOutNorm = gMix.acc.voiceToOut[0].Global().CV();
  output.audio.AddMul(voiceMixdown, voiceToOutNorm);
  for (int i = 0; i < FFEffectCount; i++)
  {
    auto const& gfxToOutNorm = gMix.acc.GFXToOut[i].Global().CV();
    output.audio.AddMul(globalDSP.gEffect[i].output, gfxToOutNorm);
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float balPlain = moduleTopo.NormalizedToLinearFast(FFGMixParam::Bal, balNorm.CV().Get(s));
    float gainPlain = moduleTopo.NormalizedToLinearFast(FFGMixParam::Gain, gainNorm.CV().Get(s));
    for (int c = 0; c < 2; c++)
      output.audio[c].Set(s, output.audio[c].Get(s) * gainPlain * FBStereoBalance(c, balPlain));
  }

  state.moduleSlot = 0;
  state.outputParamsNormalized = &output.outputParamsNormalized;
  _procState->dsp.global.output.processor->Process(state);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    globalDSP.gMatrix.processor->EndModulationBlock(state);
    return;
  }

  auto& exchangeDSP = exchangeToGUI->global.gMix[0];
  exchangeDSP.active = true;

  auto& exchangeParams = exchangeToGUI->param.global.gMix[0];
  exchangeParams.acc.bal[0] = balNorm.CV().Last();
  exchangeParams.acc.gain[0] = gainNorm.CV().Last();
  exchangeParams.acc.voiceToOut[0] = gMix.acc.voiceToOut[0].Global().CV().Last();
  for (int r = 0; r < FFEffectCount; r++)
    exchangeParams.acc.GFXToOut[r] = gMix.acc.GFXToOut[r].Global().CV().Last();
  for (int r = 0; r < FFEffectCount; r++)
    exchangeParams.acc.voiceToGFX[r] = gMix.acc.voiceToGFX[r].Global().CV().Last();
  for (int r = 0; r < FFMixFXToFXCount; r++)
    exchangeParams.acc.GFXToGFX[r] = gMix.acc.GFXToGFX[r].Global().CV().Last();
  
  globalDSP.gMatrix.processor->EndModulationBlock(state);
}