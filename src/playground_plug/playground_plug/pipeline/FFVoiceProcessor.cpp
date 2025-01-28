#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/pipeline/FFVoiceProcessor.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>

void 
FFVoiceProcessor::BeginVoice(FBModuleProcState state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcState<FFProcState>();
  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].env[i].processor.BeginVoice(state);
  }
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
  auto* procState = state.ProcState<FFProcState>();
  auto& voiceDSP = procState->dsp.voice[voice];
  voiceDSP.output.Clear();
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.osci[i].processor.Process(state);
    voiceDSP.output.Add(voiceDSP.osci[i].output);
  }
  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    bool envFinished = voiceDSP.env[i].processor.Process(state);
    if (i == 0)
      voiceFinished = envFinished;
  }
  voiceDSP.output.Mul(voiceDSP.env[0].output);
  return voiceFinished;
}