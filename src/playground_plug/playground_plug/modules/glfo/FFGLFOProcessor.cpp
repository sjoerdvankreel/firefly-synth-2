#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

void
FFGLFOProcessor::Process(FFModuleProcState const& state)
{
  auto const& topo = state.topo->modules[FFModuleGLFO];
  auto& output = state.proc->dsp.global.glfo[state.moduleSlot].output;
  auto const& params = state.proc->param.global.glfo[state.moduleSlot];
  bool on = topo.params[FFGLFOBlockOn].NormalizedToBool(params.block.on[0].Value());

  if (!on)
  {
    output.Clear();
    return;
  }
    
  output.Transform([&](int v) { 
    auto rate = params.acc.rate[0].Global().CV(v);
    auto plainRate = topo.params[FFGLFOAccRate].NormalizedToPlainLinear(rate);
    auto phase = _phase.Next(plainRate / state.sampleRate); 
    return FBToUnipolar(xsimd::sin(phase * FBTwoPi)); });
}