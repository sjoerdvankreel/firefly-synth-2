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
  voiceDSP.output.Fill(0.0f);

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
  FB_ASSERT(FFOsciCount == FFEffectCount); // TODO for now
  for (int i = 0; i < FFEffectCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.osci[i].output.CopyTo(voiceDSP.vEffect[i].input);
    voiceDSP.vEffect[i].processor->Process<false>(state);
    voiceDSP.output.Add(voiceDSP.vEffect[i].output);
  }

  // TODO dont hardcode this to voice amp?
  voiceDSP.output.Mul(voiceDSP.env[0].output);
  return voiceFinished;
}