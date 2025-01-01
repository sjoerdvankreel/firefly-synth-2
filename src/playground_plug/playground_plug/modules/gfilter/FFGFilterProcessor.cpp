#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>

void
FFGFilterProcessor::Process(FFModuleProcState const& state)
{
  auto const& topo = state.topo->modules[(int)FFModuleType::GFilter];
  auto& output = state.proc->dsp.global.gFilter[state.moduleSlot].output;
  auto const& input = state.proc->dsp.global.gFilter[state.moduleSlot].input;
  auto const& params = state.proc->param.global.gFilter[state.moduleSlot];
  bool on = topo.params[(int)FFGFilterParam::On].NormalizedToBool(params.block.on[0].Value());
  // todo type

  if (!on)
  {
    output.CopyFrom(input);
    return;
  }

  FBFixedFloatBlock g;
  g.Transform([&](int v) {
    auto freq = params.acc.freq[0].Global().CV(v);
    return xsimd::tan(std::numbers::pi_v<float> *freq / state.sampleRate);
  });

  FBFixedFloatBlock k;
  FBFixedFloatBlock a1;
  FBFixedFloatBlock a2;
  FBFixedFloatBlock a3;
  FBFixedFloatBlock m1;
  FBFixedFloatBlock m2;
  FBFixedFloatBlock m3;

  output.Transform([&](int ch, int v) {
    auto res = params.acc.res[0].Global().CV(v);
    return input[ch][v] * res; // TODO some filtering !
  });
}