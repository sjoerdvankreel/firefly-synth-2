#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
// TODO

#include <cmath>

void
FFMasterProcessor::Process(FFModuleProcState const& state)
{
  auto const& params = state.proc->param.global.master[state.moduleSlot];
  auto& output = state.proc->dsp.global.master.output;
  auto const& input = state.proc->dsp.global.master.input;
  auto const& gain = params.acc.gain[0].Global().CV();

  for (int ch = 0; ch < 2; ch++)
    for (int v = 0; v < FBFixedBlockVectors; v++)
      output[ch][v] = input[ch][v] * gain[v];
}