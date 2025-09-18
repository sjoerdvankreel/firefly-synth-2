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
  _procState->dsp.global.master.mtsClient = hostContext->GetMTSClient();
  _procState->dsp.global.gMatrix.processor->InitBuffers(_topo);
  for (int v = 0; v < FBMaxVoices; v++)
    _procState->dsp.voice[v].vMatrix.processor->InitBuffers(_topo);
}

FBModuleProcState
FFPlugProcessor::MakeModuleState(
  FBPlugInputBlock const& input)
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
  FBPlugInputBlock const& input, int voice)
{
  auto result = MakeModuleState(input);
  result.voice = &result.input->voiceManager->Voices()[voice];
  return result;
}

void
FFPlugProcessor::ProcessGEcho(
  FBModuleProcState& state,
  FBSArray2<float, FBFixedBlockSamples, 2>& inout)
{
  auto& globalDSP = _procState->dsp.global;
  state.moduleSlot = 0;
  inout.CopyTo(globalDSP.gEcho.input);
  globalDSP.gEcho.processor->BeginVoiceOrBlock(state, false, -1, -1);
  globalDSP.gEcho.processor->Process(state, -1);
  globalDSP.gEcho.output.CopyTo(inout);
}

void
FFPlugProcessor::ProcessVoice(
  FBPlugInputBlock const& input, int voice, int releaseAt)
{
  auto state = MakeModuleVoiceState(input, voice);
  if(_procState->dsp.voice[voice].processor.Process(state, releaseAt))
    input.voiceManager->Return(voice);
}

void 
FFPlugProcessor::LeaseVoices(
  FBPlugInputBlock const& input)
{
  for (int n = 0; n < input.noteEvents->size(); n++)
    if ((*input.noteEvents)[n].on)
    {
      std::array<float, FFVNoteOnNoteRandomCount> onNoteRandomUni;
      std::array<float, FFVNoteOnNoteRandomCount> onNoteRandomNorm;
      for (int r = 0; r < FFVNoteOnNoteRandomCount; r++)
      {
        onNoteRandomUni[r] = _onNoteRandomUni.NextScalar();
        onNoteRandomNorm[r] = _onNoteRandomNorm.NextScalar();
      }

      int voice = input.voiceManager->Lease((*input.noteEvents)[n]);
      auto state = MakeModuleVoiceState(input, voice);
      _procState->dsp.voice[voice].processor.BeginVoice(state, onNoteRandomUni, onNoteRandomNorm);
    }
}

void 
FFPlugProcessor::AllocOnDemandBuffers(
  FBRuntimeTopo const* topo, FBProcStateContainer* procState)
{
  _procState->dsp.global.gEcho.processor->AllocOnDemandBuffers(topo, procState, false, _sampleRate);
  for (int i = 0; i < FFEffectCount; i++)
    _procState->dsp.global.gEffect[i].processor->AllocOnDemandBuffers<true>(topo, procState, i, false, _sampleRate);
  for (int v = 0; v < FBMaxVoices; v++)
  {
    _procState->dsp.voice[v].vEcho.processor->AllocOnDemandBuffers(topo, procState, false, _sampleRate);
    for (int i = 0; i < FFOsciCount; i++)
      _procState->dsp.voice[v].osci[i].processor->AllocOnDemandBuffers(topo, procState, i, false, _sampleRate);
    for (int i = 0; i < FFEffectCount; i++)
      _procState->dsp.voice[v].vEffect[i].processor->AllocOnDemandBuffers<false>(topo, procState, i, false, _sampleRate);
  }
}

void
FFPlugProcessor::ApplyGlobalModulation(
  FBPlugInputBlock const& input, FBModuleProcState& state, FBTopoIndices moduleIndices)
{
  state.moduleSlot = 0;
  auto& globalDSP = _procState->dsp.global;
  globalDSP.gMatrix.processor->ApplyModulation(state, moduleIndices);

  // We can get away with this because FBHostProcessor does LeaseVoices() first.
  for (int v = 0; v < FBMaxVoices; v++)
    if (input.voiceManager->IsActive(v))
    {
      state.voice = &state.input->voiceManager->Voices()[v];
      _procState->dsp.voice[v].vMatrix.processor->ApplyModulation(state, moduleIndices);
    }
}

void 
FFPlugProcessor::ProcessPreVoice(FBPlugInputBlock const& input)
{
  auto state = MakeModuleState(input);
  auto& globalDSP = _procState->dsp.global;
  auto const& globalParam = _procState->param.global;

  // manual flush
  bool flushToggle = globalParam.guiSettings[0].block.flushDelayToggle[0].Value() > 0.5f;
  if (flushToggle != _prevFlushDelayToggle)
  {
    _prevFlushDelayToggle = flushToggle;
    globalDSP.gEcho.processor->FlushDelayLines();
    for (int i = 0; i < FBMaxVoices; i++)
      _procState->dsp.voice[i].vEcho.processor->FlushDelayLines();
  }

  state.moduleSlot = 0;
  globalDSP.gMatrix.processor->BeginVoiceOrBlock(state);
  globalDSP.gMatrix.processor->BeginModulationBlock();
  globalDSP.midi.processor->Process(state);
  ApplyGlobalModulation(input, state, { (int)FFModuleType::MIDI, 0 });
  globalDSP.gNote.processor->Process(state);
  ApplyGlobalModulation(input, state, { (int)FFModuleType::GNote, 0 });
  globalDSP.master.processor->Process(state);
  ApplyGlobalModulation(input, state, { (int)FFModuleType::Master, 0 });
  for (int i = 0; i < FFLFOCount; i++)
  {
    state.moduleSlot = i;
    globalDSP.gLFO[i].processor->BeginVoiceOrBlock<true>(state, nullptr, false, -1, -1);
    globalDSP.gLFO[i].processor->Process<true>(state);
    ApplyGlobalModulation(input, state, { (int)FFModuleType::GLFO, i });
  }
}

void
FFPlugProcessor::ProcessPostVoice(
  FBPlugInputBlock const& input, FBPlugOutputBlock& output)
{
  auto state = MakeModuleState(input);
  auto& globalDSP = _procState->dsp.global;
  auto const& gMix = _procState->param.global.gMix[0];
  auto const& gEcho = _procState->param.global.gEcho[0];
  auto const& balNormIn = gMix.acc.bal[0].Global();
  auto const& ampNormIn = gMix.acc.amp[0].Global();
  auto const& lfo5ToAmpNorm = gMix.acc.lfo5ToAmp[0].Global();
  auto const& lfo6ToBalNorm = gMix.acc.lfo6ToBal[0].Global();
  auto& moduleTopo = state.topo->static_->modules[(int)FFModuleType::GMix];
  auto& gEchoModuleTopo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  FBSArray<float, FBFixedBlockSamples> ampNormModulated = {};
  FBSArray<float, FBFixedBlockSamples> balNormModulated = {};
  float gEchoTargetNorm = gEcho.block.vTargetOrGTarget[0].Value();
  auto gEchoTarget = gEchoModuleTopo.NormalizedToListFast<FFGEchoTarget>(FFEchoParam::VTargetOrGTarget, gEchoTargetNorm);

  FBSArray2<float, FBFixedBlockSamples, 2> voiceMixdown = {};
  voiceMixdown.Fill(0.0f);
  for (int v = 0; v < FBMaxVoices; v++)
    if (input.voiceManager->IsActive(v))
      voiceMixdown.Add(_procState->dsp.voice[v].output);

  if (gEchoTarget == FFGEchoTarget::BeforeFX)
    ProcessGEcho(state, voiceMixdown);

  for (int i = 0; i < FFEffectCount; i++)
  {
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
    
    if(i == 0 && gEchoTarget == FFGEchoTarget::BeforeFX1)
      ProcessGEcho(state, globalDSP.gEffect[i].input);
    else if (i == 1 && gEchoTarget == FFGEchoTarget::BeforeFX2)
      ProcessGEcho(state, globalDSP.gEffect[i].input);
    else if (i == 2 && gEchoTarget == FFGEchoTarget::BeforeFX3)
      ProcessGEcho(state, globalDSP.gEffect[i].input);
    else if (i == 3 && gEchoTarget == FFGEchoTarget::BeforeFX4)
      ProcessGEcho(state, globalDSP.gEffect[i].input);
    
    state.moduleSlot = i; // gecho changes it!
    globalDSP.gEffect[i].processor->BeginVoiceOrBlock<true>(state, false, -1, -1);
    globalDSP.gEffect[i].processor->Process<true>(state);

    if (i == 0 && gEchoTarget == FFGEchoTarget::AfterFX1)
      ProcessGEcho(state, globalDSP.gEffect[i].output);
    else if (i == 1 && gEchoTarget == FFGEchoTarget::AfterFX2)
      ProcessGEcho(state, globalDSP.gEffect[i].output);
    else if (i == 2 && gEchoTarget == FFGEchoTarget::AfterFX3)
      ProcessGEcho(state, globalDSP.gEffect[i].output);
    else if (i == 3 && gEchoTarget == FFGEchoTarget::AfterFX4)
      ProcessGEcho(state, globalDSP.gEffect[i].output);
  }

  output.audio.Fill(0.0f);
  auto const& voiceToOutNorm = gMix.acc.voiceToOut[0].Global().CV();
  output.audio.AddMul(voiceMixdown, voiceToOutNorm);
  for (int i = 0; i < FFEffectCount; i++)
  {
    auto const& gfxToOutNorm = gMix.acc.GFXToOut[i].Global().CV();
    output.audio.AddMul(globalDSP.gEffect[i].output, gfxToOutNorm);
  }

  if (gEchoTarget == FFGEchoTarget::AfterFX)
    ProcessGEcho(state, output.audio);

  ampNormIn.CV().CopyTo(ampNormModulated);
  balNormIn.CV().CopyTo(balNormModulated);
  FFApplyModulation(FFModulationOpType::UPMul, globalDSP.gLFO[4].outputAll, lfo5ToAmpNorm.CV(), ampNormModulated);
  FFApplyModulation(FFModulationOpType::BPStack, globalDSP.gLFO[5].outputAll, lfo6ToBalNorm.CV(), balNormModulated);
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float balPlain = moduleTopo.NormalizedToLinearFast(FFGMixParam::Bal, balNormModulated.Get(s));
    float ampPlain = moduleTopo.NormalizedToLinearFast(FFGMixParam::Amp, ampNormModulated.Get(s));
    for (int c = 0; c < 2; c++)
      output.audio[c].Set(s, output.audio[c].Get(s) * ampPlain * FBStereoBalance(c, balPlain));
  }

  if (gEchoTarget == FFGEchoTarget::AfterMix)
    ProcessGEcho(state, output.audio);

  state.moduleSlot = 0;
  state.outputParamsNormalized = &output.outputParamsNormalized;
  _procState->dsp.global.output.processor->Process(state, output);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    globalDSP.gMatrix.processor->EndModulationBlock(state);
    return;
  }

  auto& exchangeDSP = exchangeToGUI->global.gMix[0];
  exchangeDSP.boolIsActive = 1;

  auto& exchangeParams = exchangeToGUI->param.global.gMix[0];
  exchangeParams.acc.bal[0] = balNormModulated.Last();
  exchangeParams.acc.amp[0] = ampNormModulated.Last();
  exchangeParams.acc.lfo5ToAmp[0] = lfo5ToAmpNorm.Last();
  exchangeParams.acc.lfo6ToBal[0] = lfo6ToBalNorm.Last();
  exchangeParams.acc.voiceToOut[0] = gMix.acc.voiceToOut[0].Global().CV().Last();
  for (int r = 0; r < FFEffectCount; r++)
    exchangeParams.acc.GFXToOut[r] = gMix.acc.GFXToOut[r].Global().CV().Last();
  for (int r = 0; r < FFEffectCount; r++)
    exchangeParams.acc.voiceToGFX[r] = gMix.acc.voiceToGFX[r].Global().CV().Last();
  for (int r = 0; r < FFMixFXToFXCount; r++)
    exchangeParams.acc.GFXToGFX[r] = gMix.acc.GFXToGFX[r].Global().CV().Last();
  
  globalDSP.gMatrix.processor->EndModulationBlock(state);
}