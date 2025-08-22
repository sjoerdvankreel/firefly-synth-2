#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/echo/FFGEchoTopo.hpp>
#include <firefly_synth/modules/echo/FFGEchoProcessor.hpp>

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

static std::array<float, FFGEchoReverbAllPassCount> const ReverbAllpassLength = {
  556.0f / 44100.0f, 441.0f / 44100.0f, 341.0f / 44100.0f, 225.0f / 44100.0f };
static std::array<float, FFGEchoReverbCombCount> const ReverbCombLength = {
  1116.0f / 44100.0f, 1188.0f / 44100.0f, 1277.0f / 44100.0f, 1356.0f / 44100.0f,
  1422.0f / 44100.0f, 1491.0f / 44100.0f, 1557.0f / 44100.0f, 1617.0f / 44100.0f };

void 
FFGEchoDelayState::Reset()
{
  lpFilter.Reset();
  hpFilter.Reset();
  delayLine[0].Reset(delayLine[0].MaxBufferSize());
  delayLine[1].Reset(delayLine[1].MaxBufferSize());
}

void
FFGEchoProcessor::ReleaseOnDemandBuffers(
  FBRuntimeTopo const*, FBProcStateContainer* state)
{
  for (int c = 0; c < 2; c++)
  {
    _feedbackDelayState.delayLine[c].ReleaseBuffers(state->MemoryPool());
    for (int t = 0; t < FFGEchoTapCount; t++)
      _tapDelayStates[t].delayLine[c].ReleaseBuffers(state->MemoryPool());
    for (int i = 0; i < FFGEchoReverbCombCount; i++)
    {
      _reverbState.combState[c][i].clear();
      _reverbState.combState[c][i].shrink_to_fit();
    }
    for (int i = 0; i < FFGEchoReverbAllPassCount; i++)
    {
      _reverbState.allPassState[c][i].clear();
      _reverbState.allPassState[c][i].shrink_to_fit();
    }
  }
}

void
FFGEchoProcessor::AllocOnDemandBuffers(
  FBRuntimeTopo const* topo, FBProcStateContainer* state, bool graph, float sampleRate)
{
  // for graphing we are toying with the parameters to fit the plots
  // just make it easy and allocate it all, we'll release soon and sample rate is low anyway

  auto* procState = state->RawAs<FFProcState>();
  auto const& params = procState->param.global.gEcho[0];
  auto const& tapOnNorm = params.block.tapOn;
  auto const& tapsOnNorm = params.block.tapsOn[0].Value();
  auto const& targetNorm = params.block.target[0].Value();
  auto const& reverbOnNorm = params.block.reverbOn[0].Value();
  auto const& feedbackOnNorm = params.block.feedbackOn[0].Value();
  auto const& moduleTopo = topo->static_->modules[(int)FFModuleType::GEcho];

  if (moduleTopo.NormalizedToListFast<FFGEchoTarget>(FFGEchoParam::Target, targetNorm) == FFGEchoTarget::Off)
    return;

  int maxSamples = (int)std::ceil(sampleRate * FFGEchoMaxSeconds);
  bool feedbackOn = moduleTopo.NormalizedToBoolFast(FFGEchoParam::FeedbackOn, feedbackOnNorm);
  if(graph || feedbackOn)
    for (int c = 0; c < 2; c++)
      if(_feedbackDelayState.delayLine[c].AllocBuffersIfChanged(state->MemoryPool(), maxSamples))
        _feedbackDelayState.delayLine[c].Reset(_feedbackDelayState.delayLine[c].MaxBufferSize());

  bool reverbOn = moduleTopo.NormalizedToBoolFast(FFGEchoParam::ReverbOn, reverbOnNorm);
  if(graph || reverbOn)
  {
    for (int i = 0; i < FFGEchoReverbCombCount; i++)
    {
      float combSamplesL = ReverbCombLength[i] * sampleRate;
      float combSamplesR = (ReverbCombLength[i] + ReverbSpread) * sampleRate;
      _reverbState.combState[0][i].resize((int)std::ceil(combSamplesL));
      _reverbState.combState[1][i].resize((int)std::ceil(combSamplesR));
    }
    for (int i = 0; i < FFGEchoReverbAllPassCount; i++)
    {
      float allPassSamplesL = ReverbAllpassLength[i] * sampleRate;
      float allPassSamplesR = (ReverbAllpassLength[i] + ReverbSpread) * sampleRate;
      _reverbState.allPassState[0][i].resize((int)std::ceil(allPassSamplesL));
      _reverbState.allPassState[1][i].resize((int)std::ceil(allPassSamplesR));
    }
  }

  bool tapsOn = moduleTopo.NormalizedToBoolFast(FFGEchoParam::TapsOn, tapsOnNorm);
  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    bool tapOn = moduleTopo.NormalizedToBoolFast(FFGEchoParam::TapOn, tapOnNorm[t].Value());
    if (graph || (tapsOn && tapOn))
      for (int c = 0; c < 2; c++)
        if (_tapDelayStates[t].delayLine[c].AllocBuffersIfChanged(state->MemoryPool(), maxSamples))
          _tapDelayStates[t].delayLine[c].Reset(_tapDelayStates[t].delayLine[c].MaxBufferSize());
  }
}

void 
FFGEchoProcessor::BeginBlock(
  bool graph, int graphIndex, 
  int graphSampleCount, FBModuleProcState& state)
{
  float bpm = state.input->bpm;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  float syncNorm = params.block.sync[0].Value();
  float targetNorm = params.block.target[0].Value();
  float delaySmoothTimeNorm = params.block.delaySmoothTime[0].Value();
  float delaySmoothBarsNorm = params.block.delaySmoothBars[0].Value();

  auto const& tapOnNorm = params.block.tapOn;
  auto const& tapDelayBarsNorm = params.block.tapDelayBars;
  float tapsOnNorm = params.block.tapsOn[0].Value();
  float reverbOnNorm = params.block.reverbOn[0].Value();
  float feedbackOnNorm = params.block.feedbackOn[0].Value();
  float feedbackDelayBarsNorm = params.block.feedbackDelayBars[0].Value();

  _graph = graph;
  _graphSamplesProcessed = 0;
  _graphSampleCount = graphSampleCount;
  _graphStVarFilterFreqMultiplier = FFGraphFilterFreqMultiplier(graph, state.input->sampleRate, FFMaxStateVariableFilterFreq);

  _sync = topo.NormalizedToBoolFast(FFGEchoParam::Sync, syncNorm);
  _target = topo.NormalizedToListFast<FFGEchoTarget>(FFGEchoParam::Target, targetNorm);

  float delaySmoothSamples;
  if (_sync)
    delaySmoothSamples = topo.NormalizedToBarsFloatSamplesFast(
      FFGEchoParam::DelaySmoothBars, delaySmoothBarsNorm, sampleRate, bpm);
  else
    delaySmoothSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
      FFGEchoParam::DelaySmoothTime, delaySmoothTimeNorm, sampleRate);

  _reverbOn = topo.NormalizedToBoolFast(FFGEchoParam::ReverbOn, reverbOnNorm);
  _reverbOn &= !graph || graphIndex == 2 || graphIndex == 3;

  _feedbackOn = topo.NormalizedToBoolFast(FFGEchoParam::FeedbackOn, feedbackOnNorm);
  _feedbackOn &= !graph || graphIndex == 1 || graphIndex == 3;

  _feedbackDelayBarsSamples = topo.NormalizedToBarsFloatSamplesFast(
    FFGEchoParam::FeedbackDelayBars, feedbackDelayBarsNorm, sampleRate, bpm);
  _feedbackDelayState.smoother.SetCoeffs((int)std::ceil(delaySmoothSamples));

  if (_graph)
    _feedbackDelayState.Reset();

  _tapsOn = topo.NormalizedToBoolFast(FFGEchoParam::TapsOn, tapsOnNorm);
  _tapsOn &= !graph || graphIndex == 0 || graphIndex == 3;
  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    _tapOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapOn, tapOnNorm[t].Value());
    _tapDelayBarsSamples[t] = topo.NormalizedToBarsFloatSamplesFast(
      FFGEchoParam::TapDelayBars, tapDelayBarsNorm[t].Value(), sampleRate, bpm);
    _tapDelayStates[t].smoother.SetCoeffs((int)std::ceil(delaySmoothSamples));

    if (_graph)
      _tapDelayStates[t].Reset();
  }
}

int 
FFGEchoProcessor::Process(
  FBModuleProcState& state)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gEcho.output;
  auto const& input = procState->dsp.global.gEcho.input;
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];
  auto const& gainNorm = params.acc.gain[0].Global().CV();
  auto const& tapsMixNorm = params.acc.tapsMix[0].Global().CV();

  input.CopyTo(output);
  if (_target == FFGEchoTarget::Off)
    return 0;

  // make-up gain to offset all the dry/wet mixing
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    for (int c = 0; c < 2; c++)
      output[c].Mul(s, topo.NormalizedToLinearFast(FFGEchoParam::Gain, gainNorm.Load(s)));

  if(_tapsOn)
    ProcessTaps(state, true);
  if (_feedbackOn)
    ProcessFeedback(state, true);
  if(_reverbOn)
    ProcessReverb(state, true);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
  {
    _graphSamplesProcessed += FBFixedBlockSamples;
    return std::clamp(_graphSampleCount - _graphSamplesProcessed, 0, FBFixedBlockSamples);
  }

  auto& exchangeDSP = exchangeToGUI->global.gEcho[0];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBTimeToSamples(FFGEchoPlotLengthSeconds, sampleRate);

  auto& exchangeParams = exchangeToGUI->param.global.gEcho[0];
  exchangeParams.acc.gain[0] = gainNorm.Last();
  exchangeParams.acc.tapsMix[0] = tapsMixNorm.Last();

  // Only to push the exchange state.
  ProcessTaps(state, false);
  ProcessFeedback(state, false);
  ProcessReverb(state, false);

  return FBFixedBlockSamples;
}

void 
FFGEchoProcessor::ProcessFeedback(
  FBModuleProcState& state,
  bool processAudioOrExchangeState)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gEcho.output;
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& mixNorm = params.acc.feedbackMix[0].Global().CV();
  auto const& xOverNorm = params.acc.feedbackXOver[0].Global().CV();
  auto const& lpResNorm = params.acc.feedbackLPRes[0].Global().CV();
  auto const& hpResNorm = params.acc.feedbackHPRes[0].Global().CV();
  auto const& lpFreqNorm = params.acc.feedbackLPFreq[0].Global().CV();
  auto const& hpFreqNorm = params.acc.feedbackHPFreq[0].Global().CV();
  auto const& amountNorm = params.acc.feedbackAmount[0].Global().CV();
  auto const& delayTimeNorm = params.acc.feedbackDelayTime[0].Global().CV();

  if (processAudioOrExchangeState)
  {
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float lengthTimeSamples;
      if (_sync)
        lengthTimeSamples = _feedbackDelayBarsSamples;
      else
        lengthTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
          FFGEchoParam::FeedbackDelayTime, delayTimeNorm.Get(s), sampleRate);
      float lengthTimeSamplesSmooth = _feedbackDelayState.smoother.Next(lengthTimeSamples);

      float mixPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::FeedbackMix, mixNorm.Get(s));
      float xOverPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::FeedbackXOver, xOverNorm.Get(s));
      float amountPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::FeedbackAmount, amountNorm.Get(s));
      float lpResPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::FeedbackLPRes, lpResNorm.Get(s));
      float hpResPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::FeedbackHPRes, hpResNorm.Get(s));
      float lpFreqPlain = topo.NormalizedToLog2Fast(
        FFGEchoParam::FeedbackLPFreq, lpFreqNorm.Get(s));
      float hpFreqPlain = topo.NormalizedToLog2Fast(
        FFGEchoParam::FeedbackHPFreq, hpFreqNorm.Get(s));

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

  auto& exchangeParams = exchangeToGUI->param.global.gEcho[0];
  exchangeParams.acc.feedbackLPRes[0] = lpResNorm.Last();
  exchangeParams.acc.feedbackLPFreq[0] = lpFreqNorm.Last();
  exchangeParams.acc.feedbackHPRes[0] = hpResNorm.Last();
  exchangeParams.acc.feedbackHPFreq[0] = hpFreqNorm.Last();
  exchangeParams.acc.feedbackMix[0] = mixNorm.Last();
  exchangeParams.acc.feedbackXOver[0] = xOverNorm.Last();
  exchangeParams.acc.feedbackAmount[0] = amountNorm.Last();
  exchangeParams.acc.feedbackDelayTime[0] = delayTimeNorm.Last();
}

void
FFGEchoProcessor::ProcessTaps(
  FBModuleProcState& state, 
  bool processAudioOrExchangeState)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gEcho.output;
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& tapsMixNorm = params.acc.tapsMix;
  auto const& tapLevelNorm = params.acc.tapLevel;
  auto const& tapBalNorm = params.acc.tapBalance;
  auto const& tapXOverNorm = params.acc.tapXOver;
  auto const& tapLPResNorm = params.acc.tapLPRes;
  auto const& tapHPResNorm = params.acc.tapHPRes;
  auto const& tapLPFreqNorm = params.acc.tapLPFreq;
  auto const& tapHPFreqNorm = params.acc.tapHPFreq;
  auto const& tapDelayTimeNorm = params.acc.tapDelayTime;

  if (processAudioOrExchangeState)
  {
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float thisTapsOut[2];
      thisTapsOut[0] = 0.0f;
      thisTapsOut[1] = 0.0f;

      float tapsMixPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::TapsMix, tapsMixNorm[0].Global().CV().Get(s));

      for (int t = 0; t < FFGEchoTapCount; t++)
      {
        if (_tapOn[t])
        {
          float lengthTimeSamples;
          if (_sync)
            lengthTimeSamples = _tapDelayBarsSamples[t];
          else
            lengthTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
              FFGEchoParam::TapDelayTime, tapDelayTimeNorm[t].Global().CV().Get(s), sampleRate);
          float lengthTimeSamplesSmooth = _tapDelayStates[t].smoother.Next(lengthTimeSamples);

          float tapBalPlain = topo.NormalizedToLinearFast(
            FFGEchoParam::TapBalance, tapBalNorm[t].Global().CV().Get(s));
          float tapXOverPlain = topo.NormalizedToIdentityFast(
            FFGEchoParam::TapXOver, tapXOverNorm[t].Global().CV().Get(s));
          float tapLevelPlain = topo.NormalizedToIdentityFast(
            FFGEchoParam::TapLevel, tapLevelNorm[t].Global().CV().Get(s));
          float tapLPResPlain = topo.NormalizedToIdentityFast(
            FFGEchoParam::TapLPRes, tapLPResNorm[t].Global().CV().Get(s));
          float tapHPResPlain = topo.NormalizedToIdentityFast(
            FFGEchoParam::TapHPRes, tapHPResNorm[t].Global().CV().Get(s));
          float tapLPFreqPlain = topo.NormalizedToLog2Fast(
            FFGEchoParam::TapLPFreq, tapLPFreqNorm[t].Global().CV().Get(s));
          float tapHPFreqPlain = topo.NormalizedToLog2Fast(
            FFGEchoParam::TapHPFreq, tapHPFreqNorm[t].Global().CV().Get(s));

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

  auto& exchangeParams = exchangeToGUI->param.global.gEcho[0];
  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    exchangeParams.acc.tapLPRes[t] = tapLPResNorm[t].Global().CV().Last();
    exchangeParams.acc.tapLPFreq[t] = tapLPFreqNorm[t].Global().CV().Last();
    exchangeParams.acc.tapHPRes[t] = tapHPResNorm[t].Global().CV().Last();
    exchangeParams.acc.tapHPFreq[t] = tapHPFreqNorm[t].Global().CV().Last();
    exchangeParams.acc.tapLevel[t] = tapLevelNorm[t].Global().CV().Last();
    exchangeParams.acc.tapXOver[t] = tapXOverNorm[t].Global().CV().Last();
    exchangeParams.acc.tapBalance[t] = tapBalNorm[t].Global().CV().Last();
    exchangeParams.acc.tapDelayTime[t] = tapDelayTimeNorm[t].Global().CV().Last();
  }
}

void
FFGEchoProcessor::ProcessReverb(
  FBModuleProcState& state,
  bool processAudioOrExchangeState)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gEcho.output;
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& apfNorm = params.acc.reverbAPF[0].Global().CV();
  auto const& mixNorm = params.acc.reverbMix[0].Global().CV();
  auto const& sizeNorm = params.acc.reverbSize[0].Global().CV();
  auto const& dampNorm = params.acc.reverbDamp[0].Global().CV();
  auto const& xOverNorm = params.acc.reverbXOver[0].Global().CV();
  auto const& lpResNorm = params.acc.reverbLPRes[0].Global().CV();
  auto const& hpResNorm = params.acc.reverbHPRes[0].Global().CV();
  auto const& lpFreqNorm = params.acc.reverbLPFreq[0].Global().CV();
  auto const& hpFreqNorm = params.acc.reverbHPFreq[0].Global().CV();

  if (processAudioOrExchangeState)
  {
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float mixPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::ReverbMix, mixNorm.Get(s));
      float apfPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::ReverbAPF, apfNorm.Get(s));
      float sizePlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::ReverbSize, sizeNorm.Get(s));
      float dampPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::ReverbDamp, dampNorm.Get(s));
      float xOverPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::ReverbXOver, xOverNorm.Get(s));

#if 0 // todo
      float lpResPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::ReverbLPRes, lpResNorm.Get(s));
      float hpResPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::ReverbHPRes, hpResNorm.Get(s));
      float lpFreqPlain = topo.NormalizedToLog2Fast(
        FFGEchoParam::ReverbLPFreq, lpFreqNorm.Get(s));
      float hpFreqPlain = topo.NormalizedToLog2Fast(
        FFGEchoParam::ReverbHPFreq, hpFreqNorm.Get(s));
#endif

      // Size doesnt respond linear.
      // By just testing by listening 80% is about the midpoint.
      // Do cube root so that 0.5 ~= 0.79.
      float size = (std::cbrt(sizePlain) * ReverbRoomScale) + ReverbRoomOffset;
      float damp = dampPlain * ReverbDampScale;
      float reverbIn = output[0].Get(s) + output[1].Get(s) * ReverbGain;
      
      // TODO filter
      // TODO check default xover

      std::array<float, 2> reverbOut = { 0.0f, 0.0f };

      for (int c = 0; c < 2; c++)
      {
        for (int i = 0; i < FFGEchoReverbCombCount; i++)
        {
          int pos = _reverbState.combPosition[c][i];
          int length = (int)_reverbState.combState[c][i].size();
          float combVal = _reverbState.combState[c][i][pos];
          _reverbState.combFilter[c][i] = (combVal * (1.0f - damp)) + (_reverbState.combFilter[c][i] * damp);
          _reverbState.combState[c][i][pos] = reverbIn + (_reverbState.combFilter[c][i] * size);
          _reverbState.combPosition[c][i] = (pos + 1) % length;
          reverbOut[c] += combVal;
        }

        for (int i = 0; i < FFGEchoReverbAllPassCount; i++)
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
      float outL = reverbOut[0] * wet1 + reverbOut[1] * wet2;
      float outR = reverbOut[1] * wet1 + reverbOut[0] * wet2;
      output[0].Set(s, output[0].Get(s) * dry + outL);
      output[1].Set(s, output[1].Get(s) * dry + outR);
    }

    return;
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeParams = exchangeToGUI->param.global.gEcho[0];
  exchangeParams.acc.reverbAPF[0] = apfNorm.Last();
  exchangeParams.acc.reverbMix[0] = mixNorm.Last();
  exchangeParams.acc.reverbDamp[0] = dampNorm.Last();
  exchangeParams.acc.reverbSize[0] = sizeNorm.Last();
  exchangeParams.acc.reverbXOver[0] = xOverNorm.Last();
  exchangeParams.acc.reverbLPRes[0] = lpResNorm.Last();
  exchangeParams.acc.reverbHPRes[0] = hpResNorm.Last();
  exchangeParams.acc.reverbLPFreq[0] = lpFreqNorm.Last();
  exchangeParams.acc.reverbHPFreq[0] = hpFreqNorm.Last();
}