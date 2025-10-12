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
FFOsciProcessor::BeginVoiceExtAudio(
  FBModuleProcState& state)
{
  (void)state;

#if 0 // TODO
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];

  float waveHSModeNorm = params.block.waveHSMode[0].Voice()[voice];
  float waveDSFModeNorm = params.block.waveDSFMode[0].Voice()[voice];
  float waveDSFOverNorm = params.block.waveDSFOver[0].Voice()[voice];
  float waveDSFDistanceNorm = params.block.waveDSFDistance[0].Voice()[voice];
  _voiceStartSnapshotNorm.waveDSFBW[0] = params.voiceStart.waveDSFBW[0].Voice()[voice].CV().Get(state.voice->offsetInBlock);

  _waveHSMode = topo.NormalizedToListFast<FFOsciWaveHSMode>(FFOsciParam::WaveHSMode, waveHSModeNorm);
  _waveDSFMode = topo.NormalizedToListFast<FFOsciWaveDSFMode>(FFOsciParam::WaveDSFMode, waveDSFModeNorm);
  _waveDSFBWPlain = topo.NormalizedToLog2Fast(FFOsciParam::WaveDSFBW, _voiceStartSnapshotNorm.waveDSFBW[0]);
  _waveDSFOver = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::WaveDSFOver, waveDSFOverNorm));
  _waveDSFDistance = static_cast<float>(topo.NormalizedToDiscreteFast(FFOsciParam::WaveDSFDistance, waveDSFDistanceNorm));

  for (int i = 0; i < FFOsciWavePWCount; i++)
  {
    float wavePWModeNorm = params.block.wavePWMode[i].Voice()[voice];
    _wavePWMode[i] = topo.NormalizedToListFast<FFOsciWavePWMode>(FFOsciParam::WavePWMode, wavePWModeNorm);
  }
  for (int i = 0; i < FFOsciWaveBasicCount; i++)
  {
    float waveBasicModeNorm = params.block.waveBasicMode[i].Voice()[voice];
    _waveBasicMode[i] = topo.NormalizedToListFast<FFOsciWaveBasicMode>(FFOsciParam::WaveBasicMode, waveBasicModeNorm);
  }
  for (int u = 0; u < _uniCount; u++)
  {
    _uniWavePhaseGens[u] = FFOsciWavePhaseGenerator(uniPhaseInit.Get(u));
  }
#endif
}

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