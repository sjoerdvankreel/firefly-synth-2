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
    for (int c = 0; c < 2; c++)
      _delayLines[t][c].InitializeBuffers(maxSamples);
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
  auto const& tapLengthBarsNorm = params.block.tapLengthBars;

  auto const& syncNorm = params.block.sync[0].Value();
  auto const& reverbLPOnNorm = params.block.reverbLPOn[0].Value();
  auto const& reverbHPOnNorm = params.block.reverbHPOn[0].Value();
  auto const& reverbPlacementNorm = params.block.reverbPlacement[0].Value();

  _sync = topo.NormalizedToBoolFast(FFGEchoParam::Sync, syncNorm);
  _reverbLPOn = topo.NormalizedToBoolFast(FFGEchoParam::ReverbLPOn, reverbLPOnNorm);
  _reverbHPOn = topo.NormalizedToBoolFast(FFGEchoParam::ReverbHPOn, reverbHPOnNorm);
  _reverbPlacement = topo.NormalizedToListFast<FFGEchoReverbPlacement>(FFGEchoParam::ReverbPlacement, reverbPlacementNorm);

  for (int t = 0; t < FFGEchoTapCount; t++)
  {
    _tapOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapOn, tapOnNorm[t].Value());
    _tapLPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapLPOn, tapLPOnNorm[t].Value());
    _tapHPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapHPOn, tapHPOnNorm[t].Value());
    _tapFBLPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapFBLPOn, tapFBLPOnNorm[t].Value());
    _tapFBHPOn[t] = topo.NormalizedToBoolFast(FFGEchoParam::TapFBHPOn, tapFBHPOnNorm[t].Value());
    _tapDelayBarsSamples[t] = topo.NormalizedToBarsSamplesFast(FFGEchoParam::TapDelayBars, tapDelayBarsNorm[t].Value(), sampleRate, bpm);
    _tapLengthBarsSamples[t] = topo.NormalizedToBarsSamplesFast(FFGEchoParam::TapLengthBars, tapLengthBarsNorm[t].Value(), sampleRate, bpm);
  }
}

void 
FFGEchoProcessor::Process(FBModuleProcState& state, FBSArray2<float, FBFixedBlockSamples, 2>& inout)
{

}