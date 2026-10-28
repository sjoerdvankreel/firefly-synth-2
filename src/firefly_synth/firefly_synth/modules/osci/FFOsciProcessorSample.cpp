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
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];

  float sampleLPOnNorm = params.block.sampleLPOn[0].Voice()[voice];
  float sampleHPOnNorm = params.block.sampleHPOn[0].Voice()[voice];
  _sampleLPOn = topo.NormalizedToBoolFast(FFOsciParam::SampleLPOn, sampleLPOnNorm);
  _sampleHPOn = topo.NormalizedToBoolFast(FFOsciParam::SampleHPOn, sampleHPOnNorm);
  _sampleLPFilter.Reset();
  _sampleHPFilter.Reset();

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
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  auto const& sampleGainNorm = procParams.acc.sampleGain[0].Voice()[voice];
  auto const& sampleLPResNorm = procParams.acc.sampleLPRes[0].Voice()[voice];
  auto const& sampleHPResNorm = procParams.acc.sampleHPRes[0].Voice()[voice];
  auto const& sampleLPFreqNorm = procParams.acc.sampleLPFreq[0].Voice()[voice];
  auto const& sampleHPFreqNorm = procParams.acc.sampleHPFreq[0].Voice()[voice];

  auto const& iter = state.input->audioClips->find(FFOsciSampleSlotIds[state.moduleSlot]);
  if (iter == state.input->audioClips->end())
  {
    // todo sampler the output stuff
    _graphPosition = FFOsciSamplerGUISampleCount;
    return;
  }

  int const subBlockSize = 4;
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
      if (samplesAvailable > speedRatio * subBlockSize)
      {
        _samplerSamplesUsed[u] += _samplerInterpolators[u].process(
          speedRatio, clip.data.data() + _samplerSamplesUsed[u],
          uniOutputOversampled[u].Ptr(subBlockStart), subBlockSize, samplesAvailable, 0);

        for (int s = block * subBlockSize; s < block * subBlockSize + subBlockSize; s++)
        {
          float inputGain = topo.NormalizedToLinearFast(FFOsciParam::SampleGain, sampleGainNorm.CV().Get(s));
          uniOutputOversampled[u].Set(s, uniOutputOversampled[u].Get(s) * inputGain);
        }

        if (_sampleLPOn)
        {
          for (int s = block * subBlockSize; s < block * subBlockSize + subBlockSize; s++)
          {
            float lpFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::SampleLPFreq, sampleLPFreqNorm.CV().Get(s));
            float lpResPlain = topo.NormalizedToIdentityFast(FFOsciParam::SampleLPRes, sampleLPResNorm.CV().Get(s));
            lpFreqPlain *= _graphStVarFilterFreqMultiplier;
            _sampleLPFilter.Set(FFStateVariableFilterMode::LPF, sampleRate * _oversampleTimes, lpFreqPlain, lpResPlain, 0.0f);
            uniOutputOversampled[u].Set(s, (float)_sampleLPFilter.Next(0, uniOutputOversampled[u].Get(s)));
          }
        }

        if (_sampleHPOn)
        {
          for (int s = block * subBlockSize; s < block * subBlockSize + subBlockSize; s++)
          {
            float hpFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::SampleHPFreq, sampleHPFreqNorm.CV().Get(s));
            float hpResPlain = topo.NormalizedToIdentityFast(FFOsciParam::SampleHPRes, sampleHPResNorm.CV().Get(s));
            hpFreqPlain *= _graphStVarFilterFreqMultiplier;
            _sampleHPFilter.Set(FFStateVariableFilterMode::HPF, sampleRate * _oversampleTimes, hpFreqPlain, hpResPlain, 0.0f);
            uniOutputOversampled[u].Set(s, (float)_sampleHPFilter.Next(0, uniOutputOversampled[u].Get(s)));
          }
        }
      }
    }

  // todo sampler the output stuff
}