#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedDoubleBlock.hpp>

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

  //FBFixedDoubleBlock res;
  //res.LoadFromFloat(params.acc.res[0].Global().CV());

  FBFixedDoubleBlock g;
  g.Transform([&](int v) {
    g.LoadFromFloatAligned(params.acc.freq[0].Global().CV(v));
    //auto freq = params.acc.freq[0].Global().CV(v);
    //return xsimd::tan(std::numbers::pi * freq / state.sampleRate);
    return 0.0f;
  });

  FBFixedDoubleBlock k;
  FBFixedDoubleBlock a1;
  FBFixedDoubleBlock a2;
  FBFixedDoubleBlock a3;
  FBFixedDoubleBlock m1;
  FBFixedDoubleBlock m2;
  FBFixedDoubleBlock m3;

  output.Transform([&](int ch, int v) {
    auto resFloat = params.acc.res[0].Global().CV(v);
    //auto resDouble = xsimd::batch_cast<double>(resFloat);
    return resFloat;// 0.0f;// input[ch][v] * resDouble; // TODO some filtering !
  });
}