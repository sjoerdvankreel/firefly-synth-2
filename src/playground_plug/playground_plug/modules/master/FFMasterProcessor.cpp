#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>

void
FFMasterProcessor::Process(FFModuleProcState const& state)
{
  auto const& params = state.proc->param.global.master[state.moduleSlot];
  auto& output = state.proc->dsp.global.master.output;
  auto const& input = state.proc->dsp.global.master.input;
  auto const& gain = params.acc.gain[0].Global().CV();
  output.Transform([&](int ch, int v) { 
    return input[ch][v] * gain[v]; });
}