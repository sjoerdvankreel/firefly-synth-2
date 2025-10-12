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
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto& voiceState = procState->dsp.voice[voice];
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;
  
  FBSArray<float, FBFixedBlockSamples> audioIn;
  audioIn.Fill(FBBatch<float>(0.0f));
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    audioIn.Add(s, (*state.input->audio)[0].Load(s) * 0.5f);
    audioIn.Add(s, (*state.input->audio)[1].Load(s) * 0.5f); // TODO
  }

  if (_oversampleTimes == 1)
  {
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        uniOutputOversampled[u].Store(s, audioIn.Load(s));
    return;
  }

  for (int u = 0; u < _uniCount; u++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      _downsampledBlock.setSample(u, s, audioIn.Get(s));
  auto oversampledBlock = _oversampler.processSamplesUp(_downsampledBlock.getSubsetChannelBlock(0, _uniCount));
  for (int u = 0; u < _uniCount; u++)
    for (int s = 0; s < totalSamples; s++)
      uniOutputOversampled[u].Set(s, oversampledBlock.getSample(u, s));
}