#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/pipeline/FFVoiceProcessor.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

void 
FFVoiceProcessor::Reset()
{
  // TODO
}

void 
FFVoiceProcessor::Process(FFModuleProcState state, int voice)
{
  auto& voiceDSP = state.proc->dsp.voice[voice];
  state.voice = &state.input->voiceManager->Voices()[voice];
  voiceDSP.output.Clear();
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.osci[i].processor.Process(state);
    voiceDSP.output.Add(voiceDSP.osci[i].output);
  }
}