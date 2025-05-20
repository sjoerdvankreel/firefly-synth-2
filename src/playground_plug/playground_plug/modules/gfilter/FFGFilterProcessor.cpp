#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void
FFGFilterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.gFilter[state.moduleSlot].output;
  auto const& input = procState->dsp.global.gFilter[state.moduleSlot].input;
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GFilter];
  auto const& procParams = procState->param.global.gFilter[state.moduleSlot];
  
  auto const& onNorm = procParams.block.on[0].Value();
  auto const& modeNorm = procParams.block.mode[0].Value();
  bool on = topo.NormalizedToBoolFast(FFGFilterParam::On, onNorm);
  auto mode = topo.NormalizedToListFast<FFGFilterMode>(FFGFilterParam::Mode, modeNorm);

  if (!on)
  {
    input.CopyTo(output);
    return;
  }

  auto const& resNorm = procParams.acc.res[0].Global();
  auto const& freqNorm = procParams.acc.freq[0].Global();
  auto const& gainNorm = procParams.acc.gain[0].Global();

  FBSIMDArray<double, FBFixedBlockSamples> m0, m1, m2;
  FBSIMDArray<double, FBFixedBlockSamples> a1, a2, a3;
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDDoubleCount)
  {
    auto freqPlain = topo.NormalizedToLog2Fast(FFGFilterParam::Freq, freqNorm.CV().LoadFloatToDouble(s));
    auto resPlain = topo.NormalizedToIdentityFast(FFGFilterParam::Res, resNorm.CV().LoadFloatToDouble(s));
    auto k = 2.0 - 2.0 * resPlain;
    auto g = xsimd::tan(std::numbers::pi * freqPlain / state.input->sampleRate);

    FBBatch<double> a = {};
    if (mode == FFGFilterMode::BLL || mode == FFGFilterMode::LSH || mode == FFGFilterMode::HSH)
    {
      auto gainPlain = topo.NormalizedToLinearFast(FFGFilterParam::Gain, gainNorm.CV().LoadFloatToDouble(s));
      a = xsimd::pow(FBBatch<double>(10.0), gainPlain / 40.0);
      switch (mode)
      {
      case FFGFilterMode::BLL: k = k / a; break;
      case FFGFilterMode::LSH: g = g / xsimd::sqrt(a); break;
      case FFGFilterMode::HSH: g = g * xsimd::sqrt(a); break;
      default: assert(false); break;
      }
    }
    
    auto a1_ = 1.0 / (1.0 + g * (g + k));
    auto a2_ = g * a1_;
    auto a3_ = g * a2_;
    a1.Store(s, a1_);
    a2.Store(s, a2_);
    a3.Store(s, a3_);

    FBBatch<double> m0_(0.0);
    FBBatch<double> m1_(0.0);
    FBBatch<double> m2_(0.0);

    switch (mode)
    {
    case FFGFilterMode::LPF: m2_ = 1.0; break;
    case FFGFilterMode::BPF: m1_ = 1.0; break;
    case FFGFilterMode::HPF: m0_ = 1.0; m1_ = -k; m2_ = -1.0; break;
    case FFGFilterMode::BSF: m0_ = 1.0; m1_ = -k; break;
    case FFGFilterMode::PEQ: m0_ = 1.0; m1_ = -k; m2_ = -2.0; break;
    case FFGFilterMode::APF: m0_ = 1.0; m1_ = -2.0 * k; break;
    case FFGFilterMode::BLL: m0_ = 1.0; m1_ = k * (a * a - 1.0); break;
    case FFGFilterMode::LSH: m0_ = 1.0; m1_ = k * (a - 1.0); m2_ = a * a - 1.0; break;
    case FFGFilterMode::HSH: m0_ = a * a; m1_ = k * (1.0 - a * a); m2_ = 1.0 - a * a; break;
    default: assert(false);  break;
    }

    m0.Store(s, m0_);
    m1.Store(s, m1_);
    m2.Store(s, m2_);
  }

  for (int s = 0; s < FBFixedBlockSamples; s++)
    for (int ch = 0; ch < 2; ch++)
    {
      double v0 = input[ch].Get(s);
      double v3 = v0 - _ic2eq[ch];
      double v1 = a1.Get(s) * _ic1eq[ch] + a2.Get(s) * v3;
      double v2 = _ic2eq[ch] + a2.Get(s) * _ic1eq[ch] + a3.Get(s) * v3;
      _ic1eq[ch] = 2 * v1 - _ic1eq[ch];
      _ic2eq[ch] = 2 * v2 - _ic2eq[ch];
      output[ch].Set(s, static_cast<float>(m0.Get(s) * v0 + m1.Get(s) * v1 + m2.Get(s) * v2));
    }

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