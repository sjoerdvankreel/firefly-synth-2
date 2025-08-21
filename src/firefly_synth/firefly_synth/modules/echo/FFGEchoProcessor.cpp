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
    _feedbackDelayGlobalState.delayLine[c].ReleaseBuffers(state->MemoryPool());
  for (int t = 0; t < FFGEchoTapCount; t++)
    for (int c = 0; c < 2; c++)
    {
      _tapDelayStates[t].delayLine[c].ReleaseBuffers(state->MemoryPool());
      _feedbackDelayPerTapStates[t].delayLine[c].ReleaseBuffers(state->MemoryPool());
    }
}

void
FFGEchoProcessor::AllocOnDemandBuffers(
  FBRuntimeTopo const* topo, FBProcStateContainer* state, float sampleRate)
{
  auto* procState = state->RawAs<FFProcState>();
  auto const& params = procState->param.global.gEcho[0];
  auto const& tapOnNorm = params.block.tapOn;
  auto const& targetNorm = params.block.target[0].Value();
  auto const& feedbackTypeNorm = params.block.feedbackType[0].Value();
  auto const& moduleTopo = topo->static_->modules[(int)FFModuleType::GEcho];

  if (moduleTopo.NormalizedToListFast<FFGEchoTarget>(FFGEchoParam::Target, targetNorm) == FFGEchoTarget::Off)
    return;

  int maxSamples = (int)std::ceil(sampleRate * FFGEchoMaxSeconds);
  auto feedbackType = moduleTopo.NormalizedToListFast<FFGEchoFeedbackType>(FFGEchoParam::FeedbackType, feedbackTypeNorm);
  if(feedbackType == FFGEchoFeedbackType::Main)
    for (int c = 0; c < 2; c++)
      if(_feedbackDelayGlobalState.delayLine[c].AllocBuffersIfChanged(state->MemoryPool(), maxSamples))
        _feedbackDelayGlobalState.delayLine[c].Reset(_feedbackDelayGlobalState.delayLine[c].MaxBufferSize());

  for (int t = 0; t < FFGEchoTapCount; t++)
    if (moduleTopo.NormalizedToBoolFast(FFGEchoParam::TapOn, tapOnNorm[t].Value()))
      for (int c = 0; c < 2; c++)
      {
        if (_tapDelayStates[t].delayLine[c].AllocBuffersIfChanged(state->MemoryPool(), maxSamples))
          _tapDelayStates[t].delayLine[c].Reset(_tapDelayStates[t].delayLine[c].MaxBufferSize());
        if (feedbackType == FFGEchoFeedbackType::Taps)
          if (_feedbackDelayPerTapStates[t].delayLine[c].AllocBuffersIfChanged(state->MemoryPool(), maxSamples))
            _feedbackDelayPerTapStates[t].delayLine[c].Reset(_feedbackDelayPerTapStates[t].delayLine[c].MaxBufferSize());
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
  float feedbackTypeNorm = params.block.feedbackType[0].Value();
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

  bool feedbackOn = !graph || graphIndex == 1 || graphIndex == 3;
  _feedbackType = !feedbackOn? 
    FFGEchoFeedbackType::Off:
    graph && graphIndex == 1? FFGEchoFeedbackType::Main:
    topo.NormalizedToListFast<FFGEchoFeedbackType>(FFGEchoParam::FeedbackType, feedbackTypeNorm);
  _feedbackDelayBarsSamples = topo.NormalizedToBarsFloatSamplesFast(
    FFGEchoParam::FeedbackDelayBars, feedbackDelayBarsNorm, sampleRate, bpm);
  _feedbackDelaySmoother.SetCoeffs((int)std::ceil(delaySmoothSamples));

  if (_graph)
    _feedbackDelayGlobalState.Reset();

  bool tapsOn = !graph || graphIndex == 0 || graphIndex == 3;
  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    _tapOn[t] = tapsOn && topo.NormalizedToBoolFast(FFGEchoParam::TapOn, tapOnNorm[t].Value());
    _tapDelayBarsSamples[t] = topo.NormalizedToBarsFloatSamplesFast(
      FFGEchoParam::TapDelayBars, tapDelayBarsNorm[t].Value(), sampleRate, bpm);
    _tapDelaySmoothers[t].SetCoeffs((int)std::ceil(delaySmoothSamples));

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

  ProcessTaps(state, output, true);
  if (_feedbackType == FFGEchoFeedbackType::Main)
    ProcessFeedback(state, _feedbackDelayGlobalState, output, true);

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
  ProcessTaps(state, output, false);
  ProcessFeedback(state, _feedbackDelayGlobalState, output, false);

  return FBFixedBlockSamples;
}

void 
FFGEchoProcessor::ProcessFeedback(
  FBModuleProcState& state,
  FFGEchoDelayState& delayState,
  FBSArray2<float, FBFixedBlockSamples, 2>& inout,
  bool processAudioOrExchangeState)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
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
      float lengthTimeSamplesSmooth = _feedbackDelaySmoother.Next(lengthTimeSamples);

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
        delayState.delayLine[c].Delay(lengthTimeSamplesSmooth);
        outLR[c] = delayState.delayLine[c].PopLagrangeInterpolate();
      }

      if (_graph)
      {
        lpFreqPlain *= _graphStVarFilterFreqMultiplier;
        hpFreqPlain *= _graphStVarFilterFreqMultiplier;
      }

      delayState.lpFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreqPlain, lpResPlain, 0.0);
      delayState.hpFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, hpFreqPlain, hpResPlain, 0.0);
      for (int c = 0; c < 2; c++)
      {
        double out = (1.0f - xOverPlain) * outLR[c] + xOverPlain * outLR[c == 0 ? 1 : 0];
        out = delayState.lpFilter.Next(c, out);
        out = delayState.hpFilter.Next(c, out);

        float feedbackVal = inout[c].Get(s) + amountPlain * 0.99f * (float)out;
        // because resonant filter inside feedback path
        feedbackVal = FFSoftClip10(feedbackVal);
        delayState.delayLine[c].Push(feedbackVal);
        inout[c].Set(s, (1.0f - mixPlain) * inout[c].Get(s) + mixPlain * (float)out);
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
  FBSArray2<float, FBFixedBlockSamples, 2>& inout,
  bool processAudioOrExchangeState)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
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
    std::array<FBSArray2<float, FBFixedBlockSamples, 2>, FFGEchoTapCount> tapsOut = {};
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
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
          float lengthTimeSamplesSmooth = _tapDelaySmoothers[t].Next(lengthTimeSamples);

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
            _tapDelayStates[t].delayLine[c].Push(inout[c].Get(s));
            thisTapOut *= tapLevelPlain;
            thisTapOut *= FBStereoBalance(c, tapBalPlain);
            tapsOut[t][c].Set(s, (float)thisTapOut);
          }
        }
      }
    }

    if (_feedbackType == FFGEchoFeedbackType::Taps)
      for (int t = 0; t < FFGEchoTapCount; t++)
        if (_tapOn[t])
          ProcessFeedback(state, _feedbackDelayPerTapStates[t], tapsOut[t], true);

    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float thisTapsOut[2];
      thisTapsOut[0] = 0.0f;
      thisTapsOut[1] = 0.0f;
      float tapsMixPlain = topo.NormalizedToIdentityFast(
        FFGEchoParam::TapsMix, tapsMixNorm[0].Global().CV().Get(s));
      for (int t = 0; t < FFGEchoTapCount; t++)
        if (_tapOn[t])
          for (int c = 0; c < 2; c++)
            thisTapsOut[c] += tapsOut[t][c].Get(s);
      for (int c = 0; c < 2; c++)
        inout[c].Set(s, (1.0f - tapsMixPlain) * inout[c].Get(s) + tapsMixPlain * thisTapsOut[c]);
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