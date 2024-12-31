#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/glpf/FFGLPFProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>

void
FFGLPFProcessor::Process(FFModuleProcState const& state)
{
  auto const& topo = state.topo->modules[FFModuleGLPF];
  auto& output = state.proc->dsp.global.glpf[state.moduleSlot].output;
  auto const& input = state.proc->dsp.global.glpf[state.moduleSlot].input;
  auto const& params = state.proc->param.global.glpf[state.moduleSlot];
  bool on = topo.params[FFGLPFBlockOn].NormalizedToBool(params.block.on[0].Value());

  if (!on)
  {
    output.CopyFrom(input);
    return;
  }

  output.Transform([&](int ch, int v) {
    auto res = params.acc.res[0].Global().CV(v);
    return input[ch][v] * res; // TODO some filtering !
  });
}