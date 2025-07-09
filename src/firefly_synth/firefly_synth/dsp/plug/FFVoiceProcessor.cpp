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

void 
FFVoiceProcessor::BeginVoice(FBModuleProcState state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  FB_ASSERT(FFEnvCount == FFLFOCount);
  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].env[i].processor->BeginVoice(state);
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
}

bool 
FFVoiceProcessor::Process(FBModuleProcState state)
{
  bool voiceFinished = false;
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceDSP = procState->dsp.voice[voice];
  auto const& vMix = procState->param.voice.vMix[0];
  auto const& balNorm = vMix.acc.bal[0].Voice()[voice];
  auto const& gainNorm = vMix.acc.gain[0].Voice()[voice];
  auto& moduleTopo = state.topo->static_.modules[(int)FFModuleType::GMix];

  FB_ASSERT(FFEnvCount == FFLFOCount);
  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    int envProcessed = voiceDSP.env[i].processor->Process(state);
    if (i == 0)
      voiceFinished = envProcessed != FBFixedBlockSamples;
    voiceDSP.vLFO[i].processor->Process<false>(state);
  }

  state.moduleSlot = 0;
  voiceDSP.osciMod.processor->Process(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.osci[i].processor->Process(state);
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

  // TODO dont hardcode this to voice amp?
  voiceDSP.output.Mul(voiceDSP.env[0].output);

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float balPlain = moduleTopo.NormalizedToLinearFast(FFVMixParam::Bal, balNorm.CV().Get(s));
    float gainPlain = moduleTopo.NormalizedToLinearFast(FFVMixParam::Gain, gainNorm.CV().Get(s));
    for (int c = 0; c < 2; c++)
      voiceDSP.output[c].Set(s, voiceDSP.output[c].Get(s) * gainPlain * FBStereoBalance(c, balPlain));
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return voiceFinished;

  auto& exchangeDSP = exchangeToGUI->voice[voice].vMix[0];
  exchangeDSP.active = true;

  auto& exchangeParams = exchangeToGUI->param.voice.vMix[0];
  exchangeParams.acc.bal[0][voice] = balNorm.CV().Last();
  exchangeParams.acc.gain[0][voice] = gainNorm.CV().Last();
  for (int r = 0; r < FFMixFXToFXCount; r++)
    exchangeParams.acc.VFXToVFX[r][voice] = vMix.acc.VFXToVFX[r].Voice()[voice].CV().Last();
  for (int r = 0; r < FFVMixOsciToVFXCount; r++)
    exchangeParams.acc.osciToVFX[r][voice] = vMix.acc.osciToVFX[r].Voice()[voice].CV().Last();
  for (int r = 0; r < FFEffectCount; r++)
    exchangeParams.acc.VFXToOut[r][voice] = vMix.acc.VFXToOut[r].Voice()[voice].CV().Last();
  for(int r = 0; r < FFOsciCount; r++)
    exchangeParams.acc.osciToOut[r][voice] = vMix.acc.osciToOut[r].Voice()[voice].CV().Last();

  return voiceFinished;
}