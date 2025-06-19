#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/plug/FFVoiceProcessor.hpp>
#include <firefly_synth/modules/env/FFEnvProcessor.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>
#include <firefly_synth/modules/output/FFOutputProcessor.hpp>
#include <firefly_synth/modules/master/FFMasterProcessor.hpp>
#include <firefly_synth/modules/osci_mod/FFOsciModProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>

void 
FFVoiceProcessor::BeginVoice(FBModuleProcState state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].env[i].processor->BeginVoice(state);
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

  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    int envProcessed = voiceDSP.env[i].processor->Process(state);
    if (i == 0)
      voiceFinished = envProcessed != FBFixedBlockSamples;
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
        auto const& mixAmt = vMix.acc.osciToVFX[r].Voice()[voice].CV();
        voiceDSP.vEffect[i].input.AddMul(voiceDSP.osci[o].output, mixAmt);
      }
    for (int r = 0; r < FFMixFXToFXCount; r++)
      if (FFMixFXToFXGetTargetSlot(r) == i)
      {
        int source = FFMixFXToFXGetSourceSlot(r);
        auto const& mixAmt = vMix.acc.VFXToVFX[r].Voice()[voice].CV();
        voiceDSP.vEffect[i].input.AddMul(voiceDSP.vEffect[source].output, mixAmt);
      }
    voiceDSP.vEffect[i].processor->Process<false>(state);
  }

  voiceDSP.output.Fill(0.0f);
  for (int i = 0; i < FFOsciCount; i++)
  {
    auto const& mixAmt = vMix.acc.osciToOut[i].Voice()[voice].CV();
    voiceDSP.output.AddMul(voiceDSP.osci[i].output, mixAmt);
  }
  for (int i = 0; i < FFEffectCount; i++)
  {
    auto const& mixAmt = vMix.acc.VFXToOut[i].Voice()[voice].CV();
    voiceDSP.output.AddMul(voiceDSP.vEffect[i].output, mixAmt);
  }

  // TODO dont hardcode this to voice amp?
  voiceDSP.output.Mul(voiceDSP.env[0].output);
  return voiceFinished;
}