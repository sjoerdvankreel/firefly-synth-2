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
  auto const& params = procState->param.global.gLFO[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GLFO];
  bool on = topo.params[(int)FFGLFOParam::On].boolean.NormalizedToPlain(params.block.on[0].Value());

  if (!on)
  {
    output.Fill(0.0f);
    return;
  }
    
  output.Transform([&](int v) { 
    auto rate = params.acc.rate[0].Global().CV(v);
    auto plainRate = topo.params[(int)FFGLFOParam::Rate].linear.NormalizedToPlain(rate);
    auto phase = _phase.Next(plainRate / state.sampleRate); 
    return FBToUnipolar(xsimd::sin(phase * FBTwoPi)); });
}