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
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];

  float extAudioLPOnNorm = params.block.extAudioLPOn[0].Voice()[voice];
  float extAudioHPOnNorm = params.block.extAudioHPOn[0].Voice()[voice];
  _extAudioLPOn = topo.NormalizedToBoolFast(FFOsciParam::ExtAudioLPOn, extAudioLPOnNorm);
  _extAudioHPOn = topo.NormalizedToBoolFast(FFOsciParam::ExtAudioHPOn, extAudioHPOnNorm);

  _extAudioLPFilter.Reset();
  _extAudioHPFilter.Reset();
}

void 
FFOsciProcessor::ProcessExtAudio(
  FBModuleProcState& state)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  int totalSamples = FBFixedBlockSamples * _oversampleTimes;
  auto& voiceState = procState->dsp.voice[voice];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto& uniOutputOversampled = voiceState.osci[state.moduleSlot].uniOutputOversampled;

  auto const& extAudioLPResNorm = procParams.acc.extAudioLPRes[0].Voice()[voice];
  auto const& extAudioHPResNorm = procParams.acc.extAudioHPRes[0].Voice()[voice];
  auto const& extAudioLPFreqNorm = procParams.acc.extAudioLPFreq[0].Voice()[voice];
  auto const& extAudioHPFreqNorm = procParams.acc.extAudioHPFreq[0].Voice()[voice];
  auto const& extAudioInputBalNorm = procParams.acc.extAudioInputBal[0].Voice()[voice];
  auto const& extAudioInputGainNorm = procParams.acc.extAudioInputGain[0].Voice()[voice];

  FBSArray2<float, FBFixedBlockSamples, 2> audioIn;
  if (!_graph)
  {
    // Not graphing - we may have actual audio input.
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      audioIn[0].Set(s, (*state.input->audio)[0].Get(s));
      audioIn[1].Set(s, (*state.input->audio)[1].Get(s));
    }
  }
  else
  {
    // Graphing - audio in is silent, provide some dummy data that at least looks like audio.
    int periodCount = 16;
    int sampleCount = 256;
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float mul = 0.0f;
      float pos = _graphPosition / (float)sampleCount;
      if (pos < 0.25f) mul = pos * 4.0f;
      else if (pos < 0.5f) mul = 1.0f - ((pos - 0.25f) * 4.0f);
      else if (pos < 0.75f) mul = (pos - 0.5f) * 4.0f;
      else mul = 1.0f - ((pos - 0.75f) * 4.0f);
      float phase = _graphPhaseGen.NextScalar(1.0f / sampleCount);
      audioIn[0].Set(s, mul * std::sin(2.0f * FBPi * FBPhaseWrap(phase * periodCount)));
      audioIn[1].Set(s, mul * std::cos(2.0f * FBPi * FBPhaseWrap(phase * periodCount)));
      _graphPosition++;
    }
  }

  FBSArray<float, FBFixedBlockSamples> audioInMono;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float audioInL = audioIn[0].Get(s);
    float audioInR = audioIn[1].Get(s);
    float inputBal = topo.NormalizedToLinearFast(FFOsciParam::ExtAudioInputBal, extAudioInputBalNorm.CV().Get(s));
    float inputGain = topo.NormalizedToLinearFast(FFOsciParam::ExtAudioInputGain, extAudioInputGainNorm.CV().Get(s));
    audioInMono.Set(s, ((audioInL * FBStereoBalance(0, inputBal)) + (audioInR * FBStereoBalance(1, inputBal))) * inputGain);

    if (_extAudioLPOn)
    {
      float lpFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::ExtAudioLPFreq, extAudioLPFreqNorm.CV().Get(s));
      float lpResPlain = topo.NormalizedToIdentityFast(FFOsciParam::ExtAudioLPRes, extAudioLPResNorm.CV().Get(s));
      lpFreqPlain *= _graphStVarFilterFreqMultiplier;
      _extAudioLPFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreqPlain, lpResPlain, 0.0f);
      audioInMono.Set(s, (float)_extAudioLPFilter.Next(0, audioInMono.Get(s)));
    }

    if (_extAudioHPOn)
    {
      float hpFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::ExtAudioHPFreq, extAudioHPFreqNorm.CV().Get(s));
      float hpResPlain = topo.NormalizedToIdentityFast(FFOsciParam::ExtAudioHPRes, extAudioHPResNorm.CV().Get(s));
      hpFreqPlain *= _graphStVarFilterFreqMultiplier;
      _extAudioHPFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, hpFreqPlain, hpResPlain, 0.0f);
      audioInMono.Set(s, (float)_extAudioHPFilter.Next(0, audioInMono.Get(s)));
    }
  }

  if (_oversampleTimes == 1)
  {
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        uniOutputOversampled[u].Store(s, audioInMono.Load(s));
  }
  else
  {
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
        _downsampledBlock.setSample(u, s, audioInMono.Get(s));
    auto oversampledBlock = _oversampler.processSamplesUp(_downsampledBlock.getSubsetChannelBlock(0, _uniCount));
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < totalSamples; s++)
        uniOutputOversampled[u].Set(s, oversampledBlock.getSample(u, s));
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.extAudioLPRes[0][voice] = extAudioLPResNorm.Last();
  exchangeParams.acc.extAudioHPRes[0][voice] = extAudioHPResNorm.Last();
  exchangeParams.acc.extAudioLPFreq[0][voice] = extAudioLPFreqNorm.Last();
  exchangeParams.acc.extAudioHPFreq[0][voice] = extAudioHPFreqNorm.Last();
  exchangeParams.acc.extAudioInputBal[0][voice] = extAudioInputBalNorm.Last();
  exchangeParams.acc.extAudioInputGain[0][voice] = extAudioInputGainNorm.Last();
}