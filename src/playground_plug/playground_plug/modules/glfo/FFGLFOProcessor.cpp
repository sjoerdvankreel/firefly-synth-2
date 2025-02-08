#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void 
FFGLFOProcessor::Reset(FBModuleProcState const& state)
{
  _phase = {};
}

int
FFGLFOProcessor::Process(FBModuleProcState const& state)
{
  auto* procState = state.ProcState<FFProcState>();
  auto& output = procState->dsp.global.gLFO[state.moduleSlot].output;
  auto const& procParams = procState->param.global.gLFO[state.moduleSlot];
  auto const& rate = procParams.acc.rate[0].Global();

  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GLFO];
  auto const& rateParamLinear = topo.params[(int)FFGLFOParam::Rate].linear;
  bool on = topo.params[(int)FFGLFOParam::On].boolean.NormalizedToPlain(procParams.block.on[0].Value());

  if (!on)
  {
    output.Fill(0.0f);
    return 0;
  }

  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();
  output.Transform([&](int v) { 
    auto plainRate = rateParamLinear.NormalizedToPlain(rate.CV(v));
    auto phase = _phase.Next(plainRate / state.sampleRate);
    return FBToUnipolar(xsimd::sin(phase * FBTwoPi)); });

  auto* exchangeState = state.ExchangeState<FFExchangeState>();
  if (exchangeState == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeParams = exchangeState->param.global.gLFO[state.moduleSlot];
  exchangeParams.acc.rate[0] = rate.CV().data[FBFixedBlockSamples - 1];  
  auto& exchangeDSP = exchangeState->global.gLFO[state.moduleSlot];
  float lastRate = rate.CV().data[FBFixedBlockSamples - 1];
  exchangeDSP.active = true;
  exchangeDSP.cycleLengthSamples = rateParamLinear.NormalizedFreqToSamples(lastRate, state.sampleRate);
  exchangeDSP.cyclePositionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.cycleLengthSamples;
  return FBFixedBlockSamples;
}