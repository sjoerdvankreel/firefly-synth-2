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
FFOsciProcessor::BeginVoiceAudioIn(
  FBModuleProcState& state)
{
  (void)state;
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::Osci];

  float audioInLPOnNorm = params.block.audioInLPOn[0].Voice()[voice];
  float audioInHPOnNorm = params.block.audioInHPOn[0].Voice()[voice];
  _extAudioLPOn = topo.NormalizedToBoolFast(FFOsciParam::AudioInLPOn, audioInLPOnNorm);
  _extAudioHPOn = topo.NormalizedToBoolFast(FFOsciParam::AudioInHPOn, audioInHPOnNorm);

  _extAudioLPFilter.Reset();
  _extAudioHPFilter.Reset();
}

void 
FFOsciProcessor::ProcessAudioIn(
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

  auto const& audioInLPResNorm = procParams.acc.audioInLPRes[0].Voice()[voice];
  auto const& audioInHPResNorm = procParams.acc.audioInHPRes[0].Voice()[voice];
  auto const& audioInLPFreqNorm = procParams.acc.audioInLPFreq[0].Voice()[voice];
  auto const& audioInHPFreqNorm = procParams.acc.audioInHPFreq[0].Voice()[voice];
  auto const& audioInBalNorm = procParams.acc.audioInBal[0].Voice()[voice];
  auto const& audioInGainNorm = procParams.acc.audioInGain[0].Voice()[voice];

  FBSArray2<float, FBFixedBlockSamples, 2> audioIn;
  if (!_graph)
  {
    // Not graphing - we may have actual audio input.
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      // TODO
      audioIn[0].Set(s, (*state.input->mainAudio)[0].Get(s));
      audioIn[1].Set(s, (*state.input->mainAudio)[1].Get(s));
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
      float phase = FBPhaseWrap(_graphPhaseGen.NextScalar(1.0f / sampleCount) * periodCount);
      audioIn[0].Set(s, mul * (FBToBipolar(phase) + std::sin(2.0f * FBPi * phase)));
      audioIn[1].Set(s, mul * (FBToBipolar(phase) + std::cos(2.0f * FBPi * phase)));
      _graphPosition++;
    }
  }

  FBSArray<float, FBFixedBlockSamples> audioInMono;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float audioInL = audioIn[0].Get(s);
    float audioInR = audioIn[1].Get(s);
    float inputBal = topo.NormalizedToLinearFast(FFOsciParam::AudioInBal, audioInBalNorm.CV().Get(s));
    float inputGain = topo.NormalizedToLinearFast(FFOsciParam::AudioInGain, audioInGainNorm.CV().Get(s));
    audioInMono.Set(s, ((audioInL * FBStereoBalance(0, inputBal)) + (audioInR * FBStereoBalance(1, inputBal))) * inputGain);

    if (_extAudioLPOn)
    {
      float lpFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::AudioInLPFreq, audioInLPFreqNorm.CV().Get(s));
      float lpResPlain = topo.NormalizedToIdentityFast(FFOsciParam::AudioInLPRes, audioInLPResNorm.CV().Get(s));
      lpFreqPlain *= _graphStVarFilterFreqMultiplier;
      _extAudioLPFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreqPlain, lpResPlain, 0.0f);
      audioInMono.Set(s, (float)_extAudioLPFilter.Next(0, audioInMono.Get(s)));
    }

    if (_extAudioHPOn)
    {
      float hpFreqPlain = topo.NormalizedToLog2Fast(FFOsciParam::AudioInHPFreq, audioInHPFreqNorm.CV().Get(s));
      float hpResPlain = topo.NormalizedToIdentityFast(FFOsciParam::AudioInHPRes, audioInHPResNorm.CV().Get(s));
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
  exchangeParams.acc.audioInLPRes[0][voice] = audioInLPResNorm.Last();
  exchangeParams.acc.audioInHPRes[0][voice] = audioInHPResNorm.Last();
  exchangeParams.acc.audioInLPFreq[0][voice] = audioInLPFreqNorm.Last();
  exchangeParams.acc.audioInHPFreq[0][voice] = audioInHPFreqNorm.Last();
  exchangeParams.acc.audioInBal[0][voice] = audioInBalNorm.Last();
  exchangeParams.acc.audioInGain[0][voice] = audioInGainNorm.Last();
}