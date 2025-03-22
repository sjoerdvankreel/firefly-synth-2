#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/pipeline/FFVoiceProcessor.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>

void 
FFVoiceProcessor::BeginVoice(FBModuleProcState state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].env[i].processor.BeginVoice(state);
  }
  state.moduleSlot = 0;
  procState->dsp.voice[voice].osciAM.processor.BeginVoice(state);
  procState->dsp.voice[voice].osciFM.processor.BeginVoice(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].osci[i].processor.BeginVoice(state);
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
    int envProcessed = voiceDSP.env[i].processor.Process(state);
    if (i == 0)
      voiceFinished = envProcessed != FBFixedBlockSamples;
  }
  state.moduleSlot = 0;
  voiceDSP.osciAM.processor.Process(state);
  voiceDSP.osciFM.processor.Process(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.osci[i].processor.Process(state);
    voiceDSP.output.Add(voiceDSP.osci[i].output);
  }

  // TODO dont hardcode this to voice amp?
  voiceDSP.output.Mul(voiceDSP.env[0].output);
  return voiceFinished;
}