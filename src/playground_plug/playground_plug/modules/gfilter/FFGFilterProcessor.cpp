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

  FBFixedDoubleBlock res, freq, g, k;
  res.LoadCastFromFloatArray(params.acc.res[0].Global().CV());
  freq.LoadCastFromFloatArray(params.acc.freq[0].Global().CV());
  k.Transform([&](int v) { return 2.0 - 2.0 * res[v]; });
  g.Transform([&](int v) {
    auto plainFreq = topo.params[(int)FFGFilterParam::Freq].NormalizedToPlainLinear(freq[v]);
    return xsimd::tan(std::numbers::pi * plainFreq / state.sampleRate); });
  
  FBFixedDoubleBlock a1b, a2b, a3b;
  a1b.Transform([&](int v) { return 1.0 / (1.0 + g[v] * (g[v] + k[v])); });
  a2b.Transform([&](int v) { return g[v] * a1b[v]; });
  a3b.Transform([&](int v) { return g[v] * a2b[v]; });

  FBFixedDoubleArray a1a, a2a, a3a;
  a1b.StoreToDoubleArray(a1a);
  a2b.StoreToDoubleArray(a2a);
  a3b.StoreToDoubleArray(a3a);

  FBFixedDoubleAudioArray audioIn;
  FBFixedDoubleAudioArray audioOut = {};
  input.StoreCastToDoubleArray(audioIn);
  
  for (int s = 0; s < FBFixedBlockSamples; s++)
    for (int ch = 0; ch < 2; ch++)
    {
      double v0 = audioIn.data[ch].data[s];
      double v3 = v0 - _ic2eq[ch];
      double v1 = a1a.data[s] * _ic1eq[ch] + a2a.data[s] * v3;
      double v2 = _ic2eq[ch] + a2a.data[s] * _ic1eq[ch] + a3a.data[s] * v3;
      _ic1eq[ch] = 2 * v1 - _ic1eq[ch];
      _ic2eq[ch] = 2 * v2 - _ic2eq[ch];
      audioOut.data[ch].data[s] = v2;
    }
  output.LoadCastFromDoubleArray(audioOut);  
}