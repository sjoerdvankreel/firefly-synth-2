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

#include <cmath>

void
FFGEchoProcessor::InitializeBuffers(float sampleRate)
{
  int maxSamples = (int)std::ceil(sampleRate * FFGEchoMaxSeconds);
  _feedbackDelayTimeSmoother.SetCoeffs((int)std::ceil(0.2f * sampleRate));
  for (int c = 0; c < 2; c++)
  {
    _feedbackDelayLine[c].InitializeBuffers(maxSamples);
    _feedbackDelayLine[c].Reset(_feedbackDelayLine[c].MaxBufferSize());
  }
  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    _tapDelayTimeSmoothers[t].SetCoeffs((int)std::ceil(0.2f * sampleRate));
    for (int c = 0; c < 2; c++)
    {
      _tapDelayLines[t][c].InitializeBuffers(maxSamples);
      _tapDelayLines[t][c].Reset(_tapDelayLines[t][c].MaxBufferSize());
    }
  }
}

void 
FFGEchoProcessor::BeginBlock(FBModuleProcState& state)
{
  float bpm = state.input->bpm;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& syncNorm = params.block.sync[0].Value();
  auto const& orderNorm = params.block.order[0].Value();
  auto const& targetNorm = params.block.target[0].Value();

  auto const& feedbackOnNorm = params.block.feedbackOn[0].Value();
  auto const& feedbackDelayBarsNorm = params.block.feedbackDelayBars[0].Value();
  
  auto const& tapOnNorm = params.block.tapOn;
  auto const& tapDelayBarsNorm = params.block.tapDelayBars;

  _sync = topo.NormalizedToBoolFast(FFGEchoParam::Sync, syncNorm);
  _order = topo.NormalizedToListFast<FFGEchoOrder>(FFGEchoParam::Order, orderNorm);
  _target = topo.NormalizedToListFast<FFGEchoTarget>(FFGEchoParam::Target, targetNorm);

  _feedbackOn = topo.NormalizedToBoolFast(FFGEchoParam::FeedbackOn, feedbackOnNorm);
  _feedbackDelayBarsSamples = topo.NormalizedToBarsFloatSamplesFast(
    FFGEchoParam::FeedbackDelayBars, feedbackDelayBarsNorm, sampleRate, bpm);

  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    _tapOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapOn, tapOnNorm[t].Value());
    _tapDelayBarsSamples[t] = topo.NormalizedToBarsFloatSamplesFast(
      FFGEchoParam::TapDelayBars, tapDelayBarsNorm[t].Value(), sampleRate, bpm);
  }
}

void 
FFGEchoProcessor::Process(
  FBModuleProcState& state, 
  FBSArray2<float, FBFixedBlockSamples, 2>& inout)
{
  if (_target == FFGEchoTarget::Off)
    return;  
  //ProcessTaps(state, inout);
  ProcessFeedback(state, inout);
}

void 
FFGEchoProcessor::ProcessFeedback(
  FBModuleProcState& state,
  FBSArray2<float, FBFixedBlockSamples, 2>& inout)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  auto const& mixNorm = params.acc.feedbackMix;
  auto const& xOverNorm = params.acc.feedbackXOver;
  auto const& lpResNorm = params.acc.feedbackLPRes;
  auto const& hpResNorm = params.acc.feedbackHPRes;
  auto const& lpFreqNorm = params.acc.feedbackLPFreq;
  auto const& hpFreqNorm = params.acc.feedbackHPFreq;
  auto const& amountNorm = params.acc.feedbackAmount;
  auto const& delayTimeNorm = params.acc.feedbackDelayTime;

  if (!_feedbackOn)
    return;

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float lengthTimeSamples;
    if (_sync)
      lengthTimeSamples = _feedbackDelayBarsSamples;
    else
      lengthTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
        FFGEchoParam::FeedbackDelayTime, delayTimeNorm[0].Global().CV().Get(s), sampleRate);
    float lengthTimeSamplesSmooth = _feedbackDelayTimeSmoother.Next(lengthTimeSamples);

    float mixPlain = topo.NormalizedToIdentityFast(
      FFGEchoParam::FeedbackMix, mixNorm[0].Global().CV().Get(s));
    float xOverPlain = topo.NormalizedToIdentityFast(
      FFGEchoParam::FeedbackXOver, xOverNorm[0].Global().CV().Get(s));
    float amountPlain = topo.NormalizedToIdentityFast(
      FFGEchoParam::FeedbackAmount, amountNorm[0].Global().CV().Get(s));
    float lpResPlain = topo.NormalizedToIdentityFast(
      FFGEchoParam::FeedbackLPRes, lpResNorm[0].Global().CV().Get(s));
    float hpResPlain = topo.NormalizedToIdentityFast(
      FFGEchoParam::FeedbackHPRes, hpResNorm[0].Global().CV().Get(s));
    float lpFreqPlain = topo.NormalizedToLog2Fast(
      FFGEchoParam::FeedbackLPFreq, lpFreqNorm[0].Global().CV().Get(s));
    float hpFreqPlain = topo.NormalizedToLog2Fast(
      FFGEchoParam::FeedbackHPFreq, hpFreqNorm[0].Global().CV().Get(s));

    float outLR[2];
    for (int c = 0; c < 2; c++)
    {
      _feedbackDelayLine[c].Delay(lengthTimeSamplesSmooth);
      outLR[c] = _feedbackDelayLine[c].PopLagrangeInterpolate();
    }

    _feedbackLPFilter.Set(FFStateVariableFilterMode::LPF, sampleRate, lpFreqPlain, lpResPlain, 0.0);
    _feedbackHPFilter.Set(FFStateVariableFilterMode::HPF, sampleRate, hpFreqPlain, hpResPlain, 0.0);
    for (int c = 0; c < 2; c++)
    {
      double out = (1.0f - xOverPlain) * outLR[c] + xOverPlain * outLR[c == 0 ? 1 : 0];
      out = _feedbackLPFilter.Next(c, out);
      out = _feedbackHPFilter.Next(c, out);
      _feedbackDelayLine[c].Push(inout[c].Get(s) + amountPlain * 0.99f * (float)out);
      inout[c].Set(s, (1.0f - mixPlain) * inout[c].Get(s) + mixPlain * (float)out);
    }
  }
}

void
FFGEchoProcessor::ProcessTaps(
  FBModuleProcState& state, 
  FBSArray2<float, FBFixedBlockSamples, 2>& inout)
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

  FBSArray2<float, FBFixedBlockSamples, 2> tapsOut = {};
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
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
        float lengthTimeSamplesSmooth = _tapDelayTimeSmoothers[t].Next(lengthTimeSamples);

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
          _tapDelayLines[t][c].Delay(lengthTimeSamplesSmooth);
          thisTapOutLR[c] = _tapDelayLines[t][c].PopLagrangeInterpolate();
        }

        _tapLPFilters[t].Set(FFStateVariableFilterMode::LPF, sampleRate, tapLPFreqPlain, tapLPResPlain, 0.0);
        _tapHPFilters[t].Set(FFStateVariableFilterMode::HPF, sampleRate, tapHPFreqPlain, tapHPResPlain, 0.0);
        for (int c = 0; c < 2; c++)
        {
          double thisTapOut = (1.0f - tapXOverPlain) * thisTapOutLR[c] + tapXOverPlain * thisTapOutLR[c == 0 ? 1 : 0];
          thisTapOut = _tapLPFilters[t].Next(c, thisTapOut);
          thisTapOut = _tapHPFilters[t].Next(c, thisTapOut);
          _tapDelayLines[t][c].Push(inout[c].Get(s));
          thisTapOut *= tapLevelPlain;
          thisTapOut *= FBStereoBalance(c, tapBalPlain);
          tapsOut[c].Set(s, tapsOut[c].Get(s) + (float)thisTapOut);
        }
      }
    }

    for (int c = 0; c < 2; c++)
      inout[c].Set(s, (1.0f - tapsMixPlain) * inout[c].Get(s) + tapsMixPlain * tapsOut[c].Get(s));
  }
}