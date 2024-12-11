#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/dsp/FFGLFOProcessor.hpp>
#include <playground_plug/dsp/FFModuleProcState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>

#include <cmath>

struct FFGLFOBlock
{
  FFModuleProcState const& state;
  FBFixedVectorBlock& output;
  bool on;
  FBFixedVectorBlock const& rate;
};

void 
FFGLFOProcessor::Generate(FFGLFOBlock& block)
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    block.output.Sample(s, _phase.Next(block.state.sampleRate, 1.0f));
  for (int v = 0; v < FBFixedBlockVectors; v++)
    block.output[v] = (block.output[v] * FBFloatVector::TwoPi()).Sin().Unipolar();
}

void
FFGLFOProcessor::Process(FFModuleProcState const& state)
{
  auto const& topo = state.topo->modules[FFModuleGLFO];
  auto const& params = state.proc->param.global.glfo[state.moduleSlot];
  FFGLFOBlock block = {
    state,
    state.proc->dsp.global.glfo[state.moduleSlot].output,
    topo.params[FFGLFOBlockOn].NormalizedToBool(params.block.on[0].Value()),
    params.acc.rate[0].Global().CV() };

  if (block.on)
    Generate(block);
  else
    block.output.Clear();
}