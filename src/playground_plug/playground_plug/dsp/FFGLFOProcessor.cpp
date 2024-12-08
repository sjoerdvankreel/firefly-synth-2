#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/dsp/FFGLFOProcessor.hpp>
#include <playground_plug/dsp/FFModuleProcState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>

void
FFGLFOProcessor::Process(FFModuleProcState const& state)
{
  auto const& topo = state.topo->modules[FFModuleGLFO];
  auto const& params = state.proc->param.global.glfo[state.moduleSlot];
  auto& output = state.proc->dsp.global.glfo[state.moduleSlot].output;
  bool on = topo.params[FFGLFOBlockOn].NormalizedToBool(params.block.on[0].Value());

  if (!on)
  {
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  for (int s = 0; s < output.Count(); s++)
    output[s] = FBPhaseToSine(_phase.Next(state.sampleRate, 1.0f));
}