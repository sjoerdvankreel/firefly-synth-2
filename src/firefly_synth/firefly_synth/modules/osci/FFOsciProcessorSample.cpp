#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/osci/FFOsciProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void 
FFOsciProcessor::BeginVoiceSample(
  FBModuleProcState& state, bool graph)
{
  (void)graph;
  (void)state;
  for (int i = 0; i < _uniCount; i++)
  {
    _samplerSamplesUsed[i] = 0;
    _samplerInterpolators[i].reset();
  }
}

void 
FFOsciProcessor::ProcessSample(
  FBModuleProcState& state,
  FBSArray<float, FFOsciFixedBlockOversamples> const& basePitchPlain,
  FBSArray<float, FFOsciFixedBlockOversamples> const& uniDetunePlain)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  auto const& iter = state.input->audioClips->find(FFOsciSampleSlotIds[state.moduleSlot]);
  if (iter == state.input->audioClips->end())
  {
    _graphPosition = FFOsciSamplerGUISampleCount;
    return;
  }

  int subBlockSize = 4;
  auto const& clip = iter->second;
  float freqC4 = FBPitchToFreq(60.0f);
  _graphPosition += FBFixedBlockSamples;
  double baseSpeedRatio = _graph? 1.0: clip.sampleRate / ((double)sampleRate * _oversampleTimes);
  for (int u = 0; u < _uniCount; u++)
    for (int block = 0; block < totalSamples / subBlockSize; block++)
    {
      int subBlockStart = block * subBlockSize;
      auto uniPitch = basePitchPlain.Get(subBlockStart);
      uniPitch += _uniPosMHalfToHalf.Get(u) * uniDetunePlain.Get(subBlockStart);
      double speedRatio = baseSpeedRatio * FBPitchToFreq(uniPitch) / freqC4;
      int samplesAvailable = (int)clip.data.size() - _samplerSamplesUsed[u];
      if(samplesAvailable > speedRatio * subBlockSize)
        _samplerSamplesUsed[u] += _samplerInterpolators[u].process(
          speedRatio, clip.data.data() + _samplerSamplesUsed[u],
          uniOutputOversampled[u].Ptr(subBlockStart), subBlockSize, samplesAvailable, 0);
    }
}