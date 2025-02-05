#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>

void
FFGLFOProcessor::Process(FBModuleProcState const& state)
{
  auto* procState = state.ProcState<FFProcState>();
  auto& output = procState->dsp.global.gLFO[state.moduleSlot].output;
  auto const& procParams = procState->param.global.gLFO[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GLFO];
  bool on = topo.params[(int)FFGLFOParam::On].boolean.NormalizedToPlain(procParams.block.on[0].Value());
  auto const& rate = procParams.acc.rate[0].Global();

  if (!on)
  {
    output.Fill(0.0f);
    return;
  }
    
  output.Transform([&](int v) { 
    auto plainRate = topo.params[(int)FFGLFOParam::Rate].linear.NormalizedToPlain(rate.CV(v));
    auto phase = _phase.Next(plainRate / state.sampleRate); 
    return FBToUnipolar(xsimd::sin(phase * FBTwoPi)); });

  auto* exchangeState = state.ExchangeState<FFExchangeState>();
  if (exchangeState == nullptr)
    return;
  auto& exchangeParams = exchangeState->param.global.gLFO[state.moduleSlot];
  exchangeState->global.gLFO[state.moduleSlot].active = true;
  exchangeParams.acc.rate[0] = rate.CV().data[FBFixedBlockSamples - 1];
}