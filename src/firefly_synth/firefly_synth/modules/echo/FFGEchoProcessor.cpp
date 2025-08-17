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
  _delayTimeSmoother.SetCoeffs((int)std::ceil(0.2f * sampleRate));
  int maxSamples = (int)std::ceil(sampleRate * FFGEchoMaxSeconds);
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
    _tapLengthBarsSamples[t] = topo.NormalizedToBarsSamplesFast(FFGEchoParam::TapLengthBars, tapLengthBarsNorm[t].Value(), sampleRate, bpm);

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
  auto const& lengthTimeNorm = params.acc.tapLengthTime;

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    float lengthTimeNormSmooth = _delayTimeSmoother.Next(lengthTimeNorm[0].Global().CV().Get(s));
    float mixPlain = topo.NormalizedToIdentityFast(FFGEchoParam::Mix, mixNorm[0].Global().CV().Get(s));
    float lengthTimePlain = topo.NormalizedToLinearFast(FFGEchoParam::TapLengthTime, lengthTimeNormSmooth);
    for (int c = 0; c < 2; c++)
    {
      float realIn = inout[c].Get(s);
      _preDelayBuffer[c][_preDelayBufferPosition] = realIn;
      int preDelayPos = _preDelayBufferPosition + (int)_preDelayBuffer[c].size() - _tapDelaySamples[0];
      float delayedIn = _preDelayBuffer[c][preDelayPos % _preDelayBuffer[c].size()];
      _delayLines[0][c].Delay(lengthTimePlain * sampleRate);
      float out = _delayLines[0][c].PopLagrangeInterpolate();
      _delayLines[0][c].Push(delayedIn);
      inout[c].Set(s, (1.0f - mixPlain) * realIn + mixPlain * out);
    }
    _preDelayBufferPosition = (_preDelayBufferPosition + 1) % _preDelayBuffer[0].size();
  }
}