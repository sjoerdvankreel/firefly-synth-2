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

#if 0

  FBFixedDoubleBlock res, freq, g, k;
  res.LoadFromFloatArray(params.acc.res[0].Global().CV());
  freq.LoadFromFloat(params.acc.freq[0].Global().CV());
  k.Transform([&](int v) { return 2.0 - 2.0 * res[v]; });
  g.Transform([&](int v) {
    auto plainFreq = topo.params[(int)FFGFilterParam::Freq].NormalizedToPlainLinear(freq[v]);
    return xsimd::tan(std::numbers::pi * plainFreq / state.sampleRate); });
  
  FBFixedDoubleBlock a1, a2, a3;
  a1.Transform([&](int v) { return 1.0 / (1.0 + g[v] * (g[v] + k[v])); });
  a2.Transform([&](int v) { return g[v] * a1[v]; });
  a3.Transform([&](int v) { return g[v] * a2[v]; });

  FBFixedDoubleAudioArray audio;
  input.StoreToDoubleArray(audio);
  
#if 0

  for(int s = 0; s < FBFixedBlockSamples; s++)
  { }
    for (int ch = 0; ch < 2; ch++)
    {
      double v0 = audioIn[ch][s];
      double v3 = v0 - _ic2eq[ch];
      double v1 = _a1 * _ic1eq[ch] + _a2 * v3;
      double v2 = _ic2eq[ch] + _a2 * _ic1eq[ch] + _a3 * v3;
      _ic1eq[ch] = 2 * v1 - _ic1eq[ch];
      _ic2eq[ch] = 2 * v2 - _ic2eq[ch];
      return _m0 * v0 + _m1 * v1 + _m2 * v2;
    }

  for (int v = 0; v < FBFixedDoubleVectors; v++)
  {
    for (int i = 0; i < FBVectorDoubleCount; i++)
    {
      double v0 = in;
      double v3 = v0 - _ic2eq[ch];
      double v1 = _a1 * _ic1eq[ch] + _a2 * v3;
      double v2 = _ic2eq[ch] + _a2 * _ic1eq[ch] + _a3 * v3;
      _ic1eq[ch] = 2 * v1 - _ic1eq[ch];
      _ic2eq[ch] = 2 * v2 - _ic2eq[ch];
      return _m0 * v0 + _m1 * v1 + _m2 * v2;
    }
  }
#endif

  //for(int ch = 0; ch < 2; ch++)
}