#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciProcessor::ProcessExtAudio(
  FBModuleProcState& state)
{
  //int voice = state.voice->slot;
  //auto* procState = state.ProcAs<FFProcState>();
  //float sampleRate = state.input->sampleRate;
  //float oversampledRate = sampleRate * _oversampleTimes;
  //int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  //auto& voiceState = procState->dsp.voice[voice];
  //auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;
  
  FBSArray<float, FBFixedBlockSamples> audioIn;
  switch (_type)
  {
  case FFOsciType::ExtAudioL: 
    (*state.input->audio)[0].CopyTo(audioIn); 
    break;
  case FFOsciType::ExtAudioR:
    (*state.input->audio)[1].CopyTo(audioIn);
    break;
  case FFOsciType::ExtAudioMono:
    audioIn.Fill(FBBatch<float>(0.0f));
    //audioIn.AddMul((*state.input->audio)[0], 0.5f);
    //audioIn.AddMul((*state.input->audio)[1], 0.5f);
    break;
  default:
    FB_ASSERT(false);
    break;
  }

  if (_oversampleTimes == 1)
  {
    
  }
}