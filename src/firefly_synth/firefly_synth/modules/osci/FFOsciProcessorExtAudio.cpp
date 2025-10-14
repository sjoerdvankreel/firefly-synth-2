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
}

void 
FFOsciProcessor::ProcessExtAudio(
  FBModuleProcState& state)
{
  int voice = state.voice->slot;
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

  FBSArray<float, FBFixedBlockSamples> audioIn;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float audioInL = (*state.input->audio)[0].Get(s);
    float audioInR = (*state.input->audio)[1].Get(s);
    float inputBal = topo.NormalizedToLinearFast(FFOsciParam::ExtAudioInputBal, extAudioInputBalNorm.CV().Get(s));
    float inputGain = topo.NormalizedToLinearFast(FFOsciParam::ExtAudioInputGain, extAudioInputGainNorm.CV().Get(s));
    audioIn.Set(s, ((audioInL * FBStereoBalance(0, inputBal)) + (audioInR * FBStereoBalance(1, inputBal))) * inputGain);
  }

  if (_oversampleTimes == 1)
  {
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        uniOutputOversampled[u].Store(s, audioIn.Load(s));
  }
  else
  {
    for (int u = 0; u < _uniCount; u++)
      for (int s = 0; s < FBFixedBlockSamples; s++)
        _downsampledBlock.setSample(u, s, audioIn.Get(s));
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