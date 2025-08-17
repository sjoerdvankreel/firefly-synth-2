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
  for (int t = 0; t < FFGEchoTapCount; t++)
    _delayTimeSmoothers[t].SetCoeffs((int)std::ceil(0.2f * sampleRate));
  for (int c = 0; c < 2; c++)
  {
    _preDelayBuffer[c].resize(maxSamples);
    for (int t = 0; t < FFGEchoTapCount; t++)
    {
      _delayLines[t][c].InitializeBuffers(maxSamples);
      _delayLines[t][c].Reset(_delayLines[t][c].MaxBufferSize());
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

  auto const& tapOnNorm = params.block.tapOn;
  auto const& tapLPOnNorm = params.block.tapLPOn;
  auto const& tapHPOnNorm = params.block.tapHPOn;
  auto const& tapFBLPOnNorm = params.block.tapFBLPOn;
  auto const& tapFBHPOnNorm = params.block.tapFBHPOn;
  auto const& tapDelayBarsNorm = params.block.tapDelayBars;
  auto const& tapDelayTimeNorm = params.block.tapDelayTime;
  auto const& tapLengthBarsNorm = params.block.tapLengthBars;

  auto const& syncNorm = params.block.sync[0].Value();
  auto const& targetNorm = params.block.target[0].Value();
  auto const& reverbLPOnNorm = params.block.reverbLPOn[0].Value();
  auto const& reverbHPOnNorm = params.block.reverbHPOn[0].Value();
  auto const& reverbPlacementNorm = params.block.reverbPlacement[0].Value();

  _sync = topo.NormalizedToBoolFast(FFGEchoParam::Sync, syncNorm);
  _reverbLPOn = topo.NormalizedToBoolFast(FFGEchoParam::ReverbLPOn, reverbLPOnNorm);
  _reverbHPOn = topo.NormalizedToBoolFast(FFGEchoParam::ReverbHPOn, reverbHPOnNorm);
  _target = topo.NormalizedToListFast<FFGEchoTarget>(FFGEchoParam::Target, targetNorm);
  _reverbPlacement = topo.NormalizedToListFast<FFGEchoReverbPlacement>(FFGEchoParam::ReverbPlacement, reverbPlacementNorm);

  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    _tapOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapOn, tapOnNorm[t].Value());
    _tapLPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapLPOn, tapLPOnNorm[t].Value());
    _tapHPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapHPOn, tapHPOnNorm[t].Value());
    _tapFBLPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapFBLPOn, tapFBLPOnNorm[t].Value());
    _tapFBHPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapFBHPOn, tapFBHPOnNorm[t].Value());
    _tapLengthBarsSamples[t] = topo.NormalizedToBarsFloatSamplesFast(FFGEchoParam::TapLengthBars, tapLengthBarsNorm[t].Value(), sampleRate, bpm);

    if(_sync)
      _tapDelaySamples[t] = topo.NormalizedToBarsSamplesFast(FFGEchoParam::TapDelayBars, tapDelayBarsNorm[t].Value(), sampleRate, bpm);
    else
      _tapDelaySamples[t] = topo.NormalizedToLinearTimeSamplesFast(FFGEchoParam::TapDelayTime, tapDelayTimeNorm[t].Value(), sampleRate);
  }
}

void 
FFGEchoProcessor::Process(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout)
{
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.global.gEcho[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::GEcho];

  if (_target == FFGEchoTarget::Off)
    return;

  auto const& mixNorm = params.acc.mix;
  auto const& tapBalNorm = params.acc.tapBalance;
  auto const& tapLevelNorm = params.acc.tapLevel;
  auto const& tapFeedbackNorm = params.acc.tapFeedback;
  auto const& tapLengthTimeNorm = params.acc.tapLengthTime;
  
  FBSArray2<float, FBFixedBlockSamples, 2> tapsOut = {};
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float realIn[2];
    realIn[0] = inout[0].Get(s);
    realIn[1] = inout[1].Get(s);
    _preDelayBuffer[0][_preDelayBufferPosition] = realIn[0];
    _preDelayBuffer[1][_preDelayBufferPosition] = realIn[1];

    float mixPlain = topo.NormalizedToIdentityFast(FFGEchoParam::Mix, mixNorm[0].Global().CV().Get(s));
    for (int t = 0; t < FFGEchoTapCount; t++)
    {
      if (_tapOn[t])
      {
        float lengthTimeSamples;
        if (_sync)
          lengthTimeSamples = _tapLengthBarsSamples[t];
        else
          lengthTimeSamples = topo.NormalizedToLinearTimeFloatSamplesFast(
            FFGEchoParam::TapLengthTime, tapLengthTimeNorm[t].Global().CV().Get(s), sampleRate);

        int preDelayPos = _preDelayBufferPosition + (int)_preDelayBuffer[0].size() - _tapDelaySamples[t];
        float lengthTimeSamplesSmooth = _delayTimeSmoothers[t].Next(lengthTimeSamples);
        float tapBalPlain = topo.NormalizedToLinearFast(FFGEchoParam::TapBalance, tapBalNorm[t].Global().CV().Get(s));
        float tapLevelPlain = topo.NormalizedToIdentityFast(FFGEchoParam::TapLevel, tapLevelNorm[t].Global().CV().Get(s));
        float tapFeedbackPlain = topo.NormalizedToIdentityFast(FFGEchoParam::TapFeedback, tapFeedbackNorm[t].Global().CV().Get(s));
        for (int c = 0; c < 2; c++)
        {
          float delayedIn = _preDelayBuffer[c][preDelayPos % _preDelayBuffer[c].size()];
          _delayLines[t][c].Delay(lengthTimeSamplesSmooth);
          float thisTapOut = _delayLines[t][c].PopLagrangeInterpolate();
          _delayLines[t][c].Push(delayedIn + tapFeedbackPlain * thisTapOut * 0.99f);
          thisTapOut *= tapLevelPlain;
          thisTapOut *= FBStereoBalance(c, tapBalPlain);
          tapsOut[c].Set(s, tapsOut[c].Get(s) + thisTapOut);
        }
      }
    }

    _preDelayBufferPosition = (_preDelayBufferPosition + 1) % _preDelayBuffer[0].size();
    for(int c = 0; c < 2; c++)
      inout[c].Set(s, (1.0f - mixPlain) * realIn[c] + mixPlain * tapsOut[c].Get(s));
  }
}