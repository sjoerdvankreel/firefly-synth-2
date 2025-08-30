#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/plug/FFVoiceProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

FFVEchoTarget 
FFVoiceProcessor::GetCurrentVEchoTarget(FBModuleProcState const& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& vEcho = procState->param.voice.vEcho[0];
  auto& vEchoModuleTopo = state.topo->static_->modules[(int)FFModuleType::VEcho];
  float vEchoTargetNorm = vEcho.block.vTargetOrGTarget[0].Voice()[voice];
  return vEchoModuleTopo.NormalizedToListFast<FFVEchoTarget>(FFEchoParam::VTargetOrGTarget, vEchoTargetNorm);
}

void 
FFVoiceProcessor::BeginVoice(FBModuleProcState state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();

  state.moduleSlot = 0;
  procState->dsp.voice[voice].vMatrix.processor->BeginVoiceOrBlock(state);
  state.moduleSlot = FFAmpEnvSlot;
  procState->dsp.voice[voice].env[FFAmpEnvSlot].processor->BeginVoice(state);
  for (int i = 0; i < FFLFOCount; i++)
  {
    state.moduleSlot = i + FFEnvSlotOffset;
    procState->dsp.voice[voice].env[i + FFEnvSlotOffset].processor->BeginVoice(state);
    state.moduleSlot = i;
    procState->dsp.voice[voice].vLFO[i].processor->BeginVoiceOrBlock<false>(false, -1, -1, nullptr, state);
  }

  state.moduleSlot = 0;
  procState->dsp.voice[voice].osciMod.processor->BeginVoice(false, state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].osci[i].processor->BeginVoice(false, state);
  }
  for (int i = 0; i < FFEffectCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].vEffect[i].processor->BeginVoiceOrBlock<false>(false, -1, -1, state);
  }

  if (GetCurrentVEchoTarget(state) == FFVEchoTarget::AfterMix)
  {
    state.moduleSlot = 0;
    procState->dsp.voice[voice].vEcho.processor->BeginVoiceOrBlock(false, -1, -1, state);
  }
}

bool 
FFVoiceProcessor::Process(FBModuleProcState state, int releaseAt)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceDSP = procState->dsp.voice[voice];
  auto const& vMix = procState->param.voice.vMix[0];
  auto const& balNormIn = vMix.acc.bal[0].Voice()[voice];
  auto const& ampNormIn = vMix.acc.amp[0].Voice()[voice];
  auto const& lfo6ToBalNorm = vMix.acc.lfo6ToBal[0].Voice()[voice];
  auto const& ampEnvToAmpNorm = vMix.acc.ampEnvToAmp[0].Voice()[voice];
  auto& moduleTopo = state.topo->static_->modules[(int)FFModuleType::VMix];

  FBSArray<float, FBFixedBlockSamples> ampNormModulated = {};
  FBSArray<float, FBFixedBlockSamples> balNormModulated = {};

  state.moduleSlot = 0;
  procState->dsp.voice[voice].vMatrix.processor->BeginModulationBlock();

  state.moduleSlot = FFAmpEnvSlot;
  int ampEnvProcessed = voiceDSP.env[FFAmpEnvSlot].processor->Process(state, releaseAt);
  bool voiceFinished = ampEnvProcessed != FBFixedBlockSamples;
  state.moduleSlot = 0;
  procState->dsp.voice[voice].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::Env, FFAmpEnvSlot });

  for (int i = 0; i < FFLFOCount; i++)
  {
    state.moduleSlot = i + FFEnvSlotOffset;
    voiceDSP.env[i + FFEnvSlotOffset].processor->Process(state, releaseAt);
    state.moduleSlot = 0;
    procState->dsp.voice[voice].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::Env, i + FFEnvSlotOffset });
    state.moduleSlot = i;
    voiceDSP.vLFO[i].processor->Process<false>(state);
    state.moduleSlot = 0;
    procState->dsp.voice[voice].vMatrix.processor->ApplyModulation(state, { (int)FFModuleType::VLFO, i });
  }

  state.moduleSlot = 0;
  voiceDSP.voiceModule.processor->Process(state);
  voiceDSP.osciMod.processor->Process(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.osci[i].processor->Process(false, state);
  }

  FB_ASSERT(FFOsciCount == FFEffectCount);
  for (int i = 0; i < FFEffectCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.vEffect[i].input.Fill(0.0f);
    for (int r = 0; r < FFVMixOsciToVFXCount; r++)
      if (FFVMixOsciToVFXGetFXSlot(r) == i)
      {
        int o = FFVMixOsciToVFXGetOsciSlot(r);
        auto const& osciToVFXNorm = vMix.acc.osciToVFX[r].Voice()[voice].CV();
        voiceDSP.vEffect[i].input.AddMul(voiceDSP.osci[o].output, osciToVFXNorm);
      }
    for (int r = 0; r < FFMixFXToFXCount; r++)
      if (FFMixFXToFXGetTargetSlot(r) == i)
      {
        int source = FFMixFXToFXGetSourceSlot(r);
        auto const& vfxToVFXNorm = vMix.acc.VFXToVFX[r].Voice()[voice].CV();
        voiceDSP.vEffect[i].input.AddMul(voiceDSP.vEffect[source].output, vfxToVFXNorm);
      }
    voiceDSP.vEffect[i].processor->Process<false>(state);
  }

  voiceDSP.output.Fill(0.0f);
  for (int i = 0; i < FFOsciCount; i++)
  {
    auto const& osciToOutNorm = vMix.acc.osciToOut[i].Voice()[voice].CV();
    voiceDSP.output.AddMul(voiceDSP.osci[i].output, osciToOutNorm);
  }
  for (int i = 0; i < FFEffectCount; i++)
  {
    auto const& vfxToOutNorm = vMix.acc.VFXToOut[i].Voice()[voice].CV();
    voiceDSP.output.AddMul(voiceDSP.vEffect[i].output, vfxToOutNorm);
  }

  balNormIn.CV().CopyTo(balNormModulated);
  ampNormIn.CV().CopyTo(ampNormModulated);
  FFApplyModulation(FFModulationOpType::BPStack, voiceDSP.vLFO[5].outputAll, lfo6ToBalNorm.CV(), balNormModulated);
  FFApplyModulation(FFModulationOpType::UPMul, voiceDSP.env[FFAmpEnvSlot].output, ampEnvToAmpNorm.CV(), ampNormModulated);
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float balPlain = moduleTopo.NormalizedToLinearFast(FFVMixParam::Bal, balNormModulated.Get(s));
    float ampPlain = moduleTopo.NormalizedToLinearFast(FFVMixParam::Amp, ampNormModulated.Get(s));
    for (int c = 0; c < 2; c++)
      voiceDSP.output[c].Set(s, voiceDSP.output[c].Get(s) * ampPlain * FBStereoBalance(c, balPlain));
  }

  if (GetCurrentVEchoTarget(state) == FFVEchoTarget::AfterMix)
  {
    state.moduleSlot = 0;
    voiceDSP.output.CopyTo(voiceDSP.vEcho.input);
    int vEchoProcessed = voiceDSP.vEcho.processor->Process(state, ampEnvProcessed);
    voiceFinished = vEchoProcessed != FBFixedBlockSamples;
    voiceDSP.vEcho.output.CopyTo(voiceDSP.output);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    procState->dsp.voice[voice].vMatrix.processor->EndModulationBlock(state);
    return voiceFinished;
  }

  auto& exchangeDSP = exchangeToGUI->voice[voice].vMix[0];
  exchangeDSP.active = true;

  auto& exchangeParams = exchangeToGUI->param.voice.vMix[0];
  exchangeParams.acc.bal[0][voice] = balNormModulated.Last();
  exchangeParams.acc.amp[0][voice] = ampNormModulated.Last();
  exchangeParams.acc.lfo6ToBal[0][voice] = lfo6ToBalNorm.Last();
  exchangeParams.acc.ampEnvToAmp[0][voice] = ampEnvToAmpNorm.Last();
  for (int r = 0; r < FFMixFXToFXCount; r++)
    exchangeParams.acc.VFXToVFX[r][voice] = vMix.acc.VFXToVFX[r].Voice()[voice].CV().Last();
  for (int r = 0; r < FFVMixOsciToVFXCount; r++)
    exchangeParams.acc.osciToVFX[r][voice] = vMix.acc.osciToVFX[r].Voice()[voice].CV().Last();
  for (int r = 0; r < FFEffectCount; r++)
    exchangeParams.acc.VFXToOut[r][voice] = vMix.acc.VFXToOut[r].Voice()[voice].CV().Last();
  for(int r = 0; r < FFOsciCount; r++)
    exchangeParams.acc.osciToOut[r][voice] = vMix.acc.osciToOut[r].Voice()[voice].CV().Last();

  procState->dsp.voice[voice].vMatrix.processor->EndModulationBlock(state);
  return voiceFinished;
}