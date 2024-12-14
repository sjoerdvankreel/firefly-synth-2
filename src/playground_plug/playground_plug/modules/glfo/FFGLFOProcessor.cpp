#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>

#include <cmath>

void
FFGLFOProcessor::Process(FFModuleProcState const& state)
{
  auto const& topo = state.topo->modules[FFModuleGLFO];
  auto const& params = state.proc->param.global.glfo[state.moduleSlot];
  bool on = topo.params[FFGLFOBlockOn].NormalizedToBool(params.block.on[0].Value());
  auto& output = state.proc->dsp.global.glfo[state.moduleSlot].output;
  auto const& rate = params.acc.rate[0].Global().CV();

  if (!on)
  {
    output.Clear();
    return;
  }
    
  for (int s = 0; s < FBFixedBlockSamples; s++)
    output.Sample(s, _phase.Next(state.sampleRate, 
      topo.params[FFGLFOAccRate].NormalizedToLinearPlain(rate.Sample(s))));
  for (int v = 0; v < FBFixedBlockVectors; v++)
    output[v] = (output[v] * FBFloatVector::TwoPi()).Sin().Unipolar();
}