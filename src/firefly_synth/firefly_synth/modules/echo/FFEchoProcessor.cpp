#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_synth/modules/echo/FFEchoProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

#include <cmath>
#include <algorithm>

static float const ReverbGain = 0.015f;
static float const ReverbDryScale = 2.0f;
static float const ReverbWetScale = 3.0f;
static float const ReverbDampScale = 0.4f;
static float const ReverbRoomScale = 0.28f;
static float const ReverbRoomOffset = 0.7f;
static float const ReverbSpread = 23.0f / 44100.0f;

static std::array<float, FFEchoReverbAllPassCount> const ReverbAllpassLength = {
  556.0f / 44100.0f, 441.0f / 44100.0f, 341.0f / 44100.0f, 225.0f / 44100.0f };
static std::array<float, FFEchoReverbCombCount> const ReverbCombLength = {
  1116.0f / 44100.0f, 1188.0f / 44100.0f, 1277.0f / 44100.0f, 1356.0f / 44100.0f,
  1422.0f / 44100.0f, 1491.0f / 44100.0f, 1557.0f / 44100.0f, 1617.0f / 44100.0f };

void 
FFEchoModulatableDelayState::Reset()
{
  lpFilter.Reset();
  hpFilter.Reset();
  delayLine[0].Reset(delayLine[0].MaxBufferSize());
  delayLine[1].Reset(delayLine[1].MaxBufferSize());
}

void
FFEchoReverbState::Reset()
{
  lpFilter.Reset();
  hpFilter.Reset();
  combFilter = {};
  combPosition = {};
  allPassPosition = {};
  for (int c = 0; c < 2; c++)
  {
    for (int i = 0; i < FFEchoReverbCombCount; i++)
      std::fill(combState[c][i].begin(), combState[c][i].end(), 0.0f);
    for (int i = 0; i < FFEchoReverbAllPassCount; i++)
      std::fill(allPassState[c][i].begin(), allPassState[c][i].end(), 0.0f);
  }
}

template <bool Global>
void
FFEchoProcessor<Global>::ReleaseOnDemandBuffers(
  FBRuntimeTopo const*, FBProcStateContainer* state)
{
  for (int c = 0; c < 2; c++)
  {
    _feedbackDelayState.delayLine[c].ReleaseBuffers(state->MemoryPool());
    for (int t = 0; t < FFEchoTapCount; t++)
      _tapDelayStates[t].delayLine[c].ReleaseBuffers(state->MemoryPool());
    for (int i = 0; i < FFEchoReverbCombCount; i++)
    {
      _reverbState.combState[c][i].clear();
      _reverbState.combState[c][i].shrink_to_fit();
    }
    for (int i = 0; i < FFEchoReverbAllPassCount; i++)
    {
      _reverbState.allPassState[c][i].clear();
      _reverbState.allPassState[c][i].shrink_to_fit();
    }
  }
}

template <bool Global>
void
FFEchoProcessor<Global>::AllocOnDemandBuffers(
  FBRuntimeTopo const* topo, FBProcStateContainer* state, bool graph, float sampleRate)
{
  // for graphing we are toying with the parameters to fit the plots
  // also engine state may not match the main state
  // just make it easy and allocate it all, we'll release soon and sample rate is low anyway

  auto* procState = state->RawAs<FFProcState>();
  auto const& params = *FFSelectDualState<Global>(
    [procState]() { return &procState->param.global.gEcho[0]; },
    [procState]() { return &procState->param.voice.vEcho[0]; });
  auto const& moduleTopo = topo->static_->modules[(int)(Global ? FFModuleType::GEcho : FFModuleType::VEcho)];

  auto const& tapOnNorm = params.block.tapOn;
  float tapsOnNorm = FFSelectDualProcBlockParamNormalizedGlobal<Global>(params.block.tapsOn[0]);
  float reverbOnNorm = FFSelectDualProcBlockParamNormalizedGlobal<Global>(params.block.reverbOn[0]);
  float feedbackOnNorm = FFSelectDualProcBlockParamNormalizedGlobal<Global>(params.block.feedbackOn[0]);
  float vTargetOrGTargetNorm = FFSelectDualProcBlockParamNormalizedGlobal<Global>(params.block.vTargetOrGTarget[0]);

  if (!graph && moduleTopo.NormalizedToListFast<int>(FFEchoParam::VTargetOrGTarget, vTargetOrGTargetNorm) == 0)
    return;

  int maxSamples = (int)std::ceil(sampleRate * FFEchoMaxSeconds);
  bool feedbackOn = moduleTopo.NormalizedToBoolFast(FFEchoParam::FeedbackOn, feedbackOnNorm);
  if(graph || feedbackOn)
    for (int c = 0; c < 2; c++)
      if(_feedbackDelayState.delayLine[c].AllocBuffersIfChanged(state->MemoryPool(), maxSamples))
        _feedbackDelayState.delayLine[c].Reset(_feedbackDelayState.delayLine[c].MaxBufferSize());

  bool reverbOn = moduleTopo.NormalizedToBoolFast(FFEchoParam::ReverbOn, reverbOnNorm);
  if(graph || reverbOn)
  {
    for (int i = 0; i < FFEchoReverbCombCount; i++)
    {
      float combSamplesL = ReverbCombLength[i] * sampleRate;
      float combSamplesR = (ReverbCombLength[i] + ReverbSpread) * sampleRate;
      _reverbState.combState[0][i].resize((int)std::ceil(combSamplesL));
      _reverbState.combState[1][i].resize((int)std::ceil(combSamplesR));
    }
    for (int i = 0; i < FFEchoReverbAllPassCount; i++)
    {
      float allPassSamplesL = ReverbAllpassLength[i] * sampleRate;
      float allPassSamplesR = (ReverbAllpassLength[i] + ReverbSpread) * sampleRate;
      _reverbState.allPassState[0][i].resize((int)std::ceil(allPassSamplesL));
      _reverbState.allPassState[1][i].resize((int)std::ceil(allPassSamplesR));
    }
  }

  bool tapsOn = moduleTopo.NormalizedToBoolFast(FFEchoParam::TapsOn, tapsOnNorm);
  for (int t = 0; t < FFEchoTapCount; t++)
  {
    bool tapOn = moduleTopo.NormalizedToBoolFast(FFEchoParam::TapOn, 
      FFSelectDualProcBlockParamNormalizedGlobal<Global>(tapOnNorm[t]));
    if (graph || (tapsOn && tapOn))
      for (int c = 0; c < 2; c++)
        if (_tapDelayStates[t].delayLine[c].AllocBuffersIfChanged(state->MemoryPool(), maxSamples))
          _tapDelayStates[t].delayLine[c].Reset(_tapDelayStates[t].delayLine[c].MaxBufferSize());
  }
}

template <bool Global>
void
FFEchoProcessor<Global>::BeginVoiceOrBlock(
  bool graph, int graphIndex, 
  int graphSampleCount, FBModuleProcState& state)
{
  float bpm = state.input->bpm;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState]() { return &procState->param.global.gEcho[0]; },
    [procState]() { return &procState->param.voice.vEcho[0]; });
  auto const& topo = state.topo->static_->modules[(int)(Global ? FFModuleType::GEcho : FFModuleType::VEcho)];

  float syncNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.sync[0], voice);
  float orderNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.order[0], voice);
  float delaySmoothTimeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.delaySmoothTime[0], voice);
  float delaySmoothBarsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.delaySmoothBars[0], voice);
  float vTargetOrGTargetNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.vTargetOrGTarget[0], voice);

  auto const& tapOnNorm = params.block.tapOn;
  auto const& tapDelayBarsNorm = params.block.tapDelayBars;
  float tapsOnNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.tapsOn[0], voice);
  float reverbOnNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.reverbOn[0], voice);
  float feedbackOnNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.feedbackOn[0], voice);
  float feedbackDelayBarsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.feedbackDelayBars[0], voice);
  float feedbackDelayTimeNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackDelayTime[0], voice).First();

  bool init = _graph;
  if constexpr (!Global)
    init = true;

  _graph = graph;
  _graphSamplesProcessed = 0;
  _graphSampleCount = graphSampleCount;
  _graphStVarFilterFreqMultiplier = FFGraphFilterFreqMultiplier(graph, state.input->sampleRate, FFMaxStateVariableFilterFreq);

  _sync = topo.NormalizedToBoolFast(FFEchoParam::Sync, syncNorm);
  _order = topo.NormalizedToListFast<FFEchoOrder>(FFEchoParam::Order, orderNorm);
  _on = topo.NormalizedToListFast<int>(FFEchoParam::VTargetOrGTarget, vTargetOrGTargetNorm) != 0;

  int tapsOrder = FFEchoGetProcessingOrder(_order, FFEchoModule::Taps);
  int reverbOrder = FFEchoGetProcessingOrder(_order, FFEchoModule::Reverb);
  int feedbackOrder = FFEchoGetProcessingOrder(_order, FFEchoModule::Feedback);

  float delaySmoothSamples;
  if (_sync)
    delaySmoothSamples = topo.NormalizedToBarsFloatSamplesFast(
      FFEchoParam::DelaySmoothBars, delaySmoothBarsNorm, sampleRate, bpm);
  else
    delaySmoothSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
      FFEchoParam::DelaySmoothTime, delaySmoothTimeNorm, sampleRate);

  _reverbOn = topo.NormalizedToBoolFast(FFEchoParam::ReverbOn, reverbOnNorm);
  _reverbOn &= !graph || graphIndex == reverbOrder || graphIndex == (int)FFEchoModule::Count;

  if(init)
    _reverbState.Reset();

  _feedbackOn = topo.NormalizedToBoolFast(FFEchoParam::FeedbackOn, feedbackOnNorm);
  _feedbackOn &= !graph || graphIndex == feedbackOrder || graphIndex == (int)FFEchoModule::Count;
  _feedbackDelayBarsSamples = topo.NormalizedToBarsFloatSamplesFast(
    FFEchoParam::FeedbackDelayBars, feedbackDelayBarsNorm, sampleRate, bpm);
  _feedbackDelayState.smoother.SetCoeffs((int)std::ceil(delaySmoothSamples));

  if (init)
  {
    _feedbackDelayState.Reset();
    float feedbackDelayTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
      FFEchoParam::FeedbackDelayTime, feedbackDelayTimeNorm, sampleRate);
    float feedbackDelayInitSamples = _sync ? _feedbackDelayBarsSamples : feedbackDelayTimeSamples;
    _feedbackDelayState.smoother.State(feedbackDelayInitSamples);
  }

  _tapsOn = topo.NormalizedToBoolFast(FFEchoParam::TapsOn, tapsOnNorm);
  _tapsOn &= !graph || graphIndex == tapsOrder || graphIndex == (int)FFEchoModule::Count;
  for (int t = 0; t < FFEchoTapCount; t++)
  {
    _tapOn[t] = topo.NormalizedToBoolFast(FFEchoParam::TapOn, 
      FFSelectDualProcBlockParamNormalized<Global>(tapOnNorm[t], voice));
    _tapDelayBarsSamples[t] = topo.NormalizedToBarsFloatSamplesFast(
      FFEchoParam::TapDelayBars, 
      FFSelectDualProcBlockParamNormalized<Global>(tapDelayBarsNorm[t], voice), sampleRate, bpm);
    if constexpr (!Global)
      _voiceTapDelayTimeSamples[t] = topo.NormalizedToLinearTimeFloatSamplesFast(
        FFEchoParam::TapDelayTime,
        FFSelectDualProcBlockParamNormalized<Global>(params.block.tapDelayTime[t], voice),
        sampleRate);
    _tapDelayStates[t].smoother.SetCoeffs((int)std::ceil(delaySmoothSamples));

    if (init)
    {
      float tapDelayTimeSamples;
      _tapDelayStates[t].Reset();
      if constexpr (Global)
        tapDelayTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
          FFEchoParam::TapDelayTime,
          FFSelectDualProcAccParamNormalized<Global>(params.acc.tapDelayTime[t], voice).First(),
          sampleRate);
      else
        tapDelayTimeSamples = _voiceTapDelayTimeSamples[t];
      float tapDelayInitSamples = _sync ? _tapDelayBarsSamples[t] : tapDelayTimeSamples;
      _tapDelayStates[t].smoother.State(tapDelayInitSamples);
    }
  }

  if constexpr (!Global)
  {
    _voiceFadeSamplesProcessed = 0;
    _voiceExtendSamplesProcessed = 0;
    _voiceExtensionStage = FFEchoVoiceExtensionStage::NotStarted;
    float voiceFadeTimeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.voiceFadeTime[0], voice);
    float voiceFadeBarsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.voiceFadeBars[0], voice);
    float voiceExtendTimeNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.voiceExtendTime[0], voice);
    float voiceExtendBarsNorm = FFSelectDualProcBlockParamNormalized<Global>(params.block.voiceExtendBars[0], voice);
    if (_sync)
    {
      _voiceFadeSamples = topo.NormalizedToBarsSamplesFast(
        FFEchoParam::VoiceFadeBars, voiceFadeBarsNorm, sampleRate, bpm);
      _voiceExtendSamples = topo.NormalizedToBarsSamplesFast(
        FFEchoParam::VoiceExtendBars, voiceExtendBarsNorm, sampleRate, bpm);
    }
    else
    {
      _voiceFadeSamples = topo.NormalizedToLinearTimeSamplesFast(
        FFEchoParam::VoiceFadeTime, voiceFadeTimeNorm, sampleRate);
      _voiceExtendSamples = topo.NormalizedToLinearTimeSamplesFast(
        FFEchoParam::VoiceExtendTime, voiceExtendTimeNorm, sampleRate);
    }
  }
}

template <bool Global>
int
FFEchoProcessor<Global>::Process(
  FBModuleProcState& state, int ampEnvFinishedAt)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState]() { return &procState->param.global.gEcho[0]; },
    [procState]() { return &procState->param.voice.vEcho[0]; });
  auto& dspState = *FFSelectDualState<Global>(
    [procState]() { return &procState->dsp.global.gEcho; },
    [procState, voice]() { return &procState->dsp.voice[voice].vEcho; });
  auto& output = dspState.output;
  auto const& input = dspState.input;
  auto const& topo = state.topo->static_->modules[(int)(Global ? FFModuleType::GEcho : FFModuleType::VEcho)];

  auto const& gainNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.gain[0], voice);
  auto const& tapsMixNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.tapsMix[0], voice);

  input.CopyTo(output);
  if (!_on)
    return 0;
  if constexpr (!Global)
    if (_voiceExtensionStage == FFEchoVoiceExtensionStage::Finished)
      return 0;

  // make-up gain to offset all the dry/wet mixing
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    for (int c = 0; c < 2; c++)
      output[c].Mul(s, topo.NormalizedToLinearFast(FFEchoParam::Gain, gainNorm.CV().Load(s)));

  for (int m = 0; m < (int)FFEchoModule::Count; m++)
  {
    if(_tapsOn && FFEchoGetProcessingOrder(_order, FFEchoModule::Taps) == m)
      ProcessTaps(state, true);
    else if (_feedbackOn && FFEchoGetProcessingOrder(_order, FFEchoModule::Feedback) == m)
      ProcessFeedback(state, true);
    else if (_reverbOn && FFEchoGetProcessingOrder(_order, FFEchoModule::Reverb) == m)
      ProcessReverb(state, true);
  }

  int samplesProcessed = FBFixedBlockSamples;
  if constexpr (!Global)
  {
    // Voice extend/fade-out.
    // Not for per-voice echo we operate AFTER the amp env.
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      if (_voiceExtensionStage == FFEchoVoiceExtensionStage::NotStarted && s == ampEnvFinishedAt)
        _voiceExtensionStage = FFEchoVoiceExtensionStage::Extending;
      if (_voiceExtensionStage == FFEchoVoiceExtensionStage::Extending && _voiceExtendSamplesProcessed < _voiceExtendSamples)
        _voiceExtendSamplesProcessed++;
      if (_voiceExtensionStage == FFEchoVoiceExtensionStage::Extending && _voiceExtendSamplesProcessed == _voiceExtendSamples)
        _voiceExtensionStage = FFEchoVoiceExtensionStage::Fading;
      if (_voiceExtensionStage == FFEchoVoiceExtensionStage::Fading && _voiceFadeSamplesProcessed < _voiceFadeSamples)
      {
        float fade = 1.0f - (_voiceFadeSamplesProcessed / (float)_voiceFadeSamples);
        for (int c = 0; c < 2; c++)
          output[c].Set(s, output[c].Get(s) * fade);
        _voiceFadeSamplesProcessed++;
      }
      if (_voiceExtensionStage == FFEchoVoiceExtensionStage::Fading && _voiceFadeSamplesProcessed == _voiceFadeSamples)
      {
        _voiceExtensionStage = FFEchoVoiceExtensionStage::Finished;
        samplesProcessed = 0;
      }
      if(_voiceExtensionStage == FFEchoVoiceExtensionStage::Finished)
        for (int c = 0; c < 2; c++)
          output[c].Set(s, 0.0f);
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    _graphSamplesProcessed += FBFixedBlockSamples;
    return std::clamp(_graphSampleCount - _graphSamplesProcessed, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = *FFSelectDualState<Global>(
    [exchangeToGUI]() { return &exchangeToGUI->global.gEcho[0]; },
    [exchangeToGUI, voice]() { return &exchangeToGUI->voice[voice].vEcho[0]; });
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBTimeToSamples(FFEchoPlotLengthSeconds, sampleRate);

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI] { return &exchangeToGUI->param.global.gEcho[0]; },
    [exchangeToGUI] { return &exchangeToGUI->param.voice.vEcho[0]; });
  
  FFSelectDualExchangeState<Global>(exchangeParams.acc.gain[0], voice) = gainNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.tapsMix[0], voice) = tapsMixNorm.Last();

  // Only to push the exchange state.
  ProcessTaps(state, false);
  ProcessFeedback(state, false);
  ProcessReverb(state, false);

  return samplesProcessed;
}

template <bool Global>
void
FFEchoProcessor<Global>::ProcessFeedback(
  FBModuleProcState& state,
  bool processAudioOrExchangeState)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState]() { return &procState->param.global.gEcho[0]; },
    [procState]() { return &procState->param.voice.vEcho[0]; });
  auto& dspState = *FFSelectDualState<Global>(
    [procState]() { return &procState->dsp.global.gEcho; },
    [procState, voice]() { return &procState->dsp.voice[voice].vEcho; });
  auto& output = dspState.output;  
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& mixNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackMix[0], voice);
  auto const& xOverNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackXOver[0], voice);
  auto const& lpResNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackLPRes[0], voice);
  auto const& hpResNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackHPRes[0], voice);
  auto const& lpFreqNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackLPFreq[0], voice);
  auto const& hpFreqNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackHPFreq[0], voice);
  auto const& amountNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackAmount[0], voice);
  auto const& delayTimeNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.feedbackDelayTime[0], voice);

  if (processAudioOrExchangeState)
  {
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float lengthTimeSamples;
      if (_sync)
        lengthTimeSamples = _feedbackDelayBarsSamples;
      else
        lengthTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
          FFEchoParam::FeedbackDelayTime, delayTimeNorm.CV().Get(s), sampleRate);
      float lengthTimeSamplesSmooth = _feedbackDelayState.smoother.Next(lengthTimeSamples);

      float mixPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::FeedbackMix, mixNorm.CV().Get(s));
      float xOverPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::FeedbackXOver, xOverNorm.CV().Get(s));
      float amountPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::FeedbackAmount, amountNorm.CV().Get(s));
      float lpResPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::FeedbackLPRes, lpResNorm.CV().Get(s));
      float hpResPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::FeedbackHPRes, hpResNorm.CV().Get(s));
      float lpFreqPlain = topo.NormalizedToLog2Fast(
        FFEchoParam::FeedbackLPFreq, lpFreqNorm.CV().Get(s));
      float hpFreqPlain = topo.NormalizedToLog2Fast(
        FFEchoParam::FeedbackHPFreq, hpFreqNorm.CV().Get(s));

      float outLR[2];
      for (int c = 0; c < 2; c++)
      {
        _feedbackDelayState.delayLine[c].Delay(lengthTimeSamplesSmooth);
        outLR[c] = _feedbackDelayState.delayLine[c].PopLagrangeInterpolate();
      }

      if (_graph)
      {
        lpFreqPlain *= _graphStVarFilterFreqMultiplier;
        hpFreqPlain *= _graphStVarFilterFreqMultiplier;
      }

      _feedbackDelayState.lpFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreqPlain, lpResPlain, 0.0);
      _feedbackDelayState.hpFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, hpFreqPlain, hpResPlain, 0.0);
      for (int c = 0; c < 2; c++)
      {
        double out = (1.0f - xOverPlain) * outLR[c] + xOverPlain * outLR[c == 0 ? 1 : 0];
        out = _feedbackDelayState.lpFilter.Next(c, out);
        out = _feedbackDelayState.hpFilter.Next(c, out);

        float feedbackVal = output[c].Get(s) + amountPlain * 0.99f * (float)out;
        // because resonant filter inside feedback path
        feedbackVal = FFSoftClip10(feedbackVal);
        _feedbackDelayState.delayLine[c].Push(feedbackVal);
        output[c].Set(s, (1.0f - mixPlain) * output[c].Get(s) + mixPlain * (float)out);
      }
    }

    return;
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI] { return &exchangeToGUI->param.global.gEcho[0]; },
    [exchangeToGUI] { return &exchangeToGUI->param.voice.vEcho[0]; });

  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackLPRes[0], voice) = lpResNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackLPFreq[0], voice) = lpFreqNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackHPRes[0], voice) = hpResNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackHPFreq[0], voice) = hpFreqNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackMix[0], voice) = mixNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackXOver[0], voice) = xOverNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackAmount[0], voice) = amountNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.feedbackDelayTime[0], voice) = delayTimeNorm.Last();
}

template <bool Global>
void
FFEchoProcessor<Global>::ProcessTaps(
  FBModuleProcState& state, 
  bool processAudioOrExchangeState)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState]() { return &procState->param.global.gEcho[0]; },
    [procState]() { return &procState->param.voice.vEcho[0]; });
  auto& dspState = *FFSelectDualState<Global>(
    [procState]() { return &procState->dsp.global.gEcho; },
    [procState, voice]() { return &procState->dsp.voice[voice].vEcho; });
  auto& output = dspState.output;
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& tapLevelNorm = params.acc.tapLevel;
  auto const& tapBalNorm = params.acc.tapBalance;
  auto const& tapXOverNorm = params.acc.tapXOver;
  auto const& tapLPResNorm = params.acc.tapLPRes;
  auto const& tapHPResNorm = params.acc.tapHPRes;
  auto const& tapLPFreqNorm = params.acc.tapLPFreq;
  auto const& tapHPFreqNorm = params.acc.tapHPFreq;
  auto const& tapsMixNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.tapsMix[0], voice);

  if (processAudioOrExchangeState)
  {
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float thisTapsOut[2];
      thisTapsOut[0] = 0.0f;
      thisTapsOut[1] = 0.0f;

      float tapsMixPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::TapsMix, tapsMixNorm.CV().Get(s));

      for (int t = 0; t < FFEchoTapCount; t++)
      {
        if (_tapOn[t])
        {
          float lengthTimeSamples;
          if (_sync)
            lengthTimeSamples = _tapDelayBarsSamples[t];
          else
          {
            if constexpr (Global)
              lengthTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
                FFEchoParam::TapDelayTime,
                FFSelectDualProcAccParamNormalized<Global>(params.acc.tapDelayTime[t], voice).CV().Get(s),
                sampleRate);
            else
              lengthTimeSamples = _voiceTapDelayTimeSamples[t];
          }
          float lengthTimeSamplesSmooth = _tapDelayStates[t].smoother.Next(lengthTimeSamples);

          float tapBalPlain = topo.NormalizedToLinearFast(
            FFEchoParam::TapBalance, 
            FFSelectDualProcAccParamNormalized<Global>(tapBalNorm[t], voice).CV().Get(s));
          float tapXOverPlain = topo.NormalizedToIdentityFast(
            FFEchoParam::TapXOver,
            FFSelectDualProcAccParamNormalized<Global>(tapXOverNorm[t], voice).CV().Get(s));
          float tapLevelPlain = topo.NormalizedToIdentityFast(
            FFEchoParam::TapLevel,
            FFSelectDualProcAccParamNormalized<Global>(tapLevelNorm[t], voice).CV().Get(s));
          float tapLPResPlain = topo.NormalizedToIdentityFast(
            FFEchoParam::TapLPRes,
            FFSelectDualProcAccParamNormalized<Global>(tapLPResNorm[t], voice).CV().Get(s));
          float tapHPResPlain = topo.NormalizedToIdentityFast(
            FFEchoParam::TapHPRes,
            FFSelectDualProcAccParamNormalized<Global>(tapHPResNorm[t], voice).CV().Get(s));
          float tapLPFreqPlain = topo.NormalizedToLog2Fast(
            FFEchoParam::TapLPFreq,
            FFSelectDualProcAccParamNormalized<Global>(tapLPFreqNorm[t], voice).CV().Get(s));
          float tapHPFreqPlain = topo.NormalizedToLog2Fast(
            FFEchoParam::TapHPFreq,
            FFSelectDualProcAccParamNormalized<Global>(tapHPFreqNorm[t], voice).CV().Get(s));

          float thisTapOutLR[2];
          for (int c = 0; c < 2; c++)
          {
            _tapDelayStates[t].delayLine[c].Delay(lengthTimeSamplesSmooth);
            thisTapOutLR[c] = _tapDelayStates[t].delayLine[c].PopLagrangeInterpolate();
          }

          if (_graph)
          {
            tapLPFreqPlain *= _graphStVarFilterFreqMultiplier;
            tapHPFreqPlain *= _graphStVarFilterFreqMultiplier;
          }

          _tapDelayStates[t].lpFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, tapLPFreqPlain, tapLPResPlain, 0.0);
          _tapDelayStates[t].hpFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, tapHPFreqPlain, tapHPResPlain, 0.0);
          for (int c = 0; c < 2; c++)
          {
            double thisTapOut = (1.0f - tapXOverPlain) * thisTapOutLR[c] + tapXOverPlain * thisTapOutLR[c == 0 ? 1 : 0];
            thisTapOut = _tapDelayStates[t].lpFilter.Next(c, thisTapOut);
            thisTapOut = _tapDelayStates[t].hpFilter.Next(c, thisTapOut);
            _tapDelayStates[t].delayLine[c].Push(output[c].Get(s));
            thisTapOut *= tapLevelPlain;
            thisTapOut *= FBStereoBalance(c, tapBalPlain);
            thisTapsOut[c] += (float)thisTapOut;
          }
        }
      }

      for (int c = 0; c < 2; c++)
        output[c].Set(s, (1.0f - tapsMixPlain) * output[c].Get(s) + tapsMixPlain * thisTapsOut[c]);
    }

    return;
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI] { return &exchangeToGUI->param.global.gEcho[0]; },
    [exchangeToGUI] { return &exchangeToGUI->param.voice.vEcho[0]; });
  for (int t = 0; t < FFEchoTapCount; t++)
  {
    FFSelectDualExchangeState<Global>(exchangeParams.acc.tapLPRes[t], voice) = FFSelectDualProcAccParamNormalized<Global>(tapLPResNorm[t], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.tapLPFreq[t], voice) = FFSelectDualProcAccParamNormalized<Global>(tapLPFreqNorm[t], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.tapHPRes[t], voice) = FFSelectDualProcAccParamNormalized<Global>(tapHPResNorm[t], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.tapHPFreq[t], voice) = FFSelectDualProcAccParamNormalized<Global>(tapHPFreqNorm[t], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.tapLevel[t], voice) = FFSelectDualProcAccParamNormalized<Global>(tapLevelNorm[t], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.tapXOver[t], voice) = FFSelectDualProcAccParamNormalized<Global>(tapXOverNorm[t], voice).Last();
    FFSelectDualExchangeState<Global>(exchangeParams.acc.tapBalance[t], voice) = FFSelectDualProcAccParamNormalized<Global>(tapBalNorm[t], voice).Last();
    if constexpr (Global)
      FFSelectDualExchangeState<Global>(exchangeParams.acc.tapDelayTime[t], voice) = FFSelectDualProcAccParamNormalized<Global>(params.acc.tapDelayTime[t], voice).Last();
  }
}

template <bool Global>
void
FFEchoProcessor<Global>::ProcessReverb(
  FBModuleProcState& state,
  bool processAudioOrExchangeState)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  int voice = state.voice == nullptr ? -1 : state.voice->slot;
  auto const& params = *FFSelectDualState<Global>(
    [procState]() { return &procState->param.global.gEcho[0]; },
    [procState]() { return &procState->param.voice.vEcho[0]; });
  auto& dspState = *FFSelectDualState<Global>(
    [procState]() { return &procState->dsp.global.gEcho; },
    [procState, voice]() { return &procState->dsp.voice[voice].vEcho; });
  auto& output = dspState.output;
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& apfNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbAPF[0], voice);
  auto const& mixNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbMix[0], voice);
  auto const& sizeNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbSize[0], voice);
  auto const& dampNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbDamp[0], voice);
  auto const& xOverNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbXOver[0], voice);
  auto const& lpResNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbLPRes[0], voice);
  auto const& hpResNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbHPRes[0], voice);
  auto const& lpFreqNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbLPFreq[0], voice);
  auto const& hpFreqNorm = FFSelectDualProcAccParamNormalized<Global>(params.acc.reverbHPFreq[0], voice);

  if (processAudioOrExchangeState)
  {
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float mixPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::ReverbMix, mixNorm.CV().Get(s));
      float apfPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::ReverbAPF, apfNorm.CV().Get(s));
      float sizePlain = topo.NormalizedToIdentityFast(
        FFEchoParam::ReverbSize, sizeNorm.CV().Get(s));
      float dampPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::ReverbDamp, dampNorm.CV().Get(s));
      float xOverPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::ReverbXOver, xOverNorm.CV().Get(s));
      float lpResPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::ReverbLPRes, lpResNorm.CV().Get(s));
      float hpResPlain = topo.NormalizedToIdentityFast(
        FFEchoParam::ReverbHPRes, hpResNorm.CV().Get(s));
      float lpFreqPlain = topo.NormalizedToLog2Fast(
        FFEchoParam::ReverbLPFreq, lpFreqNorm.CV().Get(s));
      float hpFreqPlain = topo.NormalizedToLog2Fast(
        FFEchoParam::ReverbHPFreq, hpFreqNorm.CV().Get(s));

      if (_graph)
      {
        lpFreqPlain *= _graphStVarFilterFreqMultiplier;
        hpFreqPlain *= _graphStVarFilterFreqMultiplier;
      }

      // Size doesnt respond linear.
      // By just testing by listening 80% is about the midpoint.
      // Do cube root so that 0.5 ~= 0.79.
      float size = (std::cbrt(sizePlain) * ReverbRoomScale) + ReverbRoomOffset;
      float damp = dampPlain * ReverbDampScale;
      float reverbIn = (output[0].Get(s) + output[1].Get(s)) * ReverbGain;

      std::array<float, 2> reverbOut = { 0.0f, 0.0f };

      for (int c = 0; c < 2; c++)
      {
        for (int i = 0; i < FFEchoReverbCombCount; i++)
        {
          int pos = _reverbState.combPosition[c][i];
          int length = (int)_reverbState.combState[c][i].size();
          float combVal = _reverbState.combState[c][i][pos];
          _reverbState.combFilter[c][i] = (combVal * (1.0f - damp)) + (_reverbState.combFilter[c][i] * damp);
          _reverbState.combState[c][i][pos] = reverbIn + (_reverbState.combFilter[c][i] * size);
          _reverbState.combPosition[c][i] = (pos + 1) % length;
          reverbOut[c] += combVal;
        }

        for (int i = 0; i < FFEchoReverbAllPassCount; i++)
        {
          float outputIn = reverbOut[c];
          int pos = _reverbState.allPassPosition[c][i];
          int length = (int)_reverbState.allPassState[c][i].size();
          float allPassVal = _reverbState.allPassState[c][i][pos];
          reverbOut[c] = -reverbOut[c] + allPassVal;
          _reverbState.allPassState[c][i][pos] = outputIn + (allPassVal * apfPlain * 0.5f);
          _reverbState.allPassPosition[c][i] = (pos + 1) % length;
        }
      }

      float wet = mixPlain * ReverbWetScale;
      float dry = (1.0f - mixPlain) * ReverbDryScale;
      float wet1 = wet * xOverPlain;
      float wet2 = wet * (1.0f - xOverPlain);
      double outL = reverbOut[0] * wet1 + reverbOut[1] * wet2;
      double outR = reverbOut[1] * wet1 + reverbOut[0] * wet2;

      _reverbState.lpFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreqPlain, lpResPlain, 0.0);
      _reverbState.hpFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, hpFreqPlain, hpResPlain, 0.0);
      outL = _reverbState.lpFilter.Next(0, outL);
      outL = _reverbState.hpFilter.Next(0, outL);
      outR = _reverbState.lpFilter.Next(1, outR);
      outR = _reverbState.hpFilter.Next(1, outR);

      output[0].Set(s, output[0].Get(s) * dry + (float)outL);
      output[1].Set(s, output[1].Get(s) * dry + (float)outR);
    }

    return;
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeParams = *FFSelectDualState<Global>(
    [exchangeToGUI] { return &exchangeToGUI->param.global.gEcho[0]; },
    [exchangeToGUI] { return &exchangeToGUI->param.voice.vEcho[0]; });

  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbAPF[0], voice) = apfNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbMix[0], voice) = mixNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbDamp[0], voice) = dampNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbSize[0], voice) = sizeNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbXOver[0], voice) = xOverNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbLPRes[0], voice) = lpResNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbHPRes[0], voice) = hpResNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbLPFreq[0], voice) = lpFreqNorm.Last();
  FFSelectDualExchangeState<Global>(exchangeParams.acc.reverbHPFreq[0], voice) = hpFreqNorm.Last();
}

template class FFEchoProcessor<true>;
template class FFEchoProcessor<false>;