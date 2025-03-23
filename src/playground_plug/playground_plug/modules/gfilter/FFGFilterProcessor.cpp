#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedDoubleBlock.hpp>

void
FFGFilterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gFilter[state.moduleSlot].output;
  auto const& input = procState->dsp.global.gFilter[state.moduleSlot].input;
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GFilter];
  auto const& procParams = procState->param.global.gFilter[state.moduleSlot];

  if (!topo.NormalizedToBoolFast(FFGFilterParam::On, procParams.block.on[0].Value()))
  {
    output.CopyFrom(input);
    return;
  }

  FBFixedDoubleBlock a;
  FBFixedDoubleBlock resPlain;
  FBFixedDoubleBlock freqPlain;
  auto const& resNorm = procParams.acc.res[0].Global();
  auto const& freqNorm = procParams.acc.freq[0].Global();
  auto const& gainNorm = procParams.acc.gain[0].Global();
  auto mode = topo.NormalizedToListFast<FFGFilterMode>(FFGFilterParam::Mode, procParams.block.mode[0].Value());
  topo.NormalizedToLog2Fast(FFGFilterParam::Freq, freqNorm, freqPlain);
  topo.NormalizedToIdentityFast(FFGFilterParam::Res, resNorm, resPlain);

  if (mode == FFGFilterMode::BLL || mode == FFGFilterMode::LSH || mode == FFGFilterMode::HSH)
  {
    FBFixedDoubleBlock gainPlain;
    topo.NormalizedToLinearFast(FFGFilterParam::Gain, gainNorm, gainPlain);
    a.Transform([&](int v) { return xsimd::pow(FBDoubleVector(10.0), gainPlain[v] / 40.0); });
  }

  FBFixedDoubleBlock g, k;
  k.Transform([&](int v) { return 2.0 - 2.0 * resPlain[v]; });
  g.Transform([&](int v) { return xsimd::tan(std::numbers::pi * freqPlain[v] / state.input->sampleRate); });

  switch (mode)
  {
  case FFGFilterMode::BLL: k.Transform([&](int v) { return k[v] / a[v]; }); break;
  case FFGFilterMode::LSH: g.Transform([&](int v) { return g[v] / xsimd::sqrt(a[v]); }); break;
  case FFGFilterMode::HSH: g.Transform([&](int v) { return g[v] * xsimd::sqrt(a[v]); }); break;
  default: break;
  }
  
  FBFixedDoubleBlock a1b, a2b, a3b;
  a1b.Transform([&](int v) { return 1.0 / (1.0 + g[v] * (g[v] + k[v])); });
  a2b.Transform([&](int v) { return g[v] * a1b[v]; });
  a3b.Transform([&](int v) { return g[v] * a2b[v]; });

  FBFixedDoubleArray a1a, a2a, a3a;
  a1b.StoreToDoubleArray(a1a);
  a2b.StoreToDoubleArray(a2a);
  a3b.StoreToDoubleArray(a3a);

  FBFixedDoubleBlock m0b, m1b, m2b;
  switch (mode)
  {
  case FFGFilterMode::LPF:
    m0b.Transform([&](int v) { return 0.0; });
    m1b.Transform([&](int v) { return 0.0; });
    m2b.Transform([&](int v) { return 1.0; });
    break;
  case FFGFilterMode::BPF:
    m0b.Transform([&](int v) { return 0.0; });
    m1b.Transform([&](int v) { return 1.0; });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterMode::HPF:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -k[v]; });
    m2b.Transform([&](int v) { return -1.0; });
    break;
  case FFGFilterMode::BSF:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -k[v]; });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterMode::PEQ:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -k[v]; });
    m2b.Transform([&](int v) { return -2.0; });
    break;
  case FFGFilterMode::APF:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -2.0 * k[v]; });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterMode::BLL:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return k[v] * (a[v] * a[v] - 1.0); });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterMode::LSH:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return k[v] * (a[v] - 1.0); });
    m2b.Transform([&](int v) { return a[v] * a[v] - 1.0; });
    break;
  case FFGFilterMode::HSH:
    m0b.Transform([&](int v) { return a[v] * a[v]; });
    m1b.Transform([&](int v) { return k[v] * (1.0 - a[v]) * a[v]; });
    m2b.Transform([&](int v) { return 1.0 - a[v] * a[v]; });
    break;
  default:
    assert(false);
    break;
  }

  FBFixedDoubleArray m0a, m1a, m2a;
  m0b.StoreToDoubleArray(m0a);
  m1b.StoreToDoubleArray(m1a);
  m2b.StoreToDoubleArray(m2a);

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
      audioOut.data[ch].data[s] = m0a.data[s] * v0 + m1a.data[s] * v1 + m2a.data[s] * v2;
    }
  output.LoadCastFromDoubleArray(audioOut);  

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->global.gFilter[state.moduleSlot];
  exchangeDSP.active = true;

  auto& exchangeParams = exchangeToGUI->param.global.gFilter[state.moduleSlot];
  exchangeParams.acc.res[0] = resNorm.Last();
  exchangeParams.acc.freq[0] = freqNorm.Last();
  exchangeParams.acc.gain[0] = gainNorm.Last();
}