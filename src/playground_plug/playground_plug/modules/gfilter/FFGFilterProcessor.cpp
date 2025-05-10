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

  if (!topo.NormalizedToBoolFast(FFGFilterParam::On, procParams.block.on[0].Value()))
  {
    input.CopyTo(output);
    return;
  }

  auto const& resNorm = procParams.acc.res[0].Global();
  auto const& freqNorm = procParams.acc.freq[0].Global();
  auto mode = topo.NormalizedToListFast<FFGFilterMode>(FFGFilterParam::Mode, procParams.block.mode[0].Value());

  // TODO
  FBSIMDArray<double, FBFixedBlockSamples> g, k;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    double freqPlain = topo.NormalizedToLog2Fast(FFGFilterParam::Freq, freqNorm.CV().Get(s));
    double resPlain = topo.NormalizedToIdentityFast(FFGFilterParam::Res, resNorm.CV().Get(s));
    k.Set(s, 2.0 - 2.0 * resPlain);
    g.Set(s, std::tan(std::numbers::pi * freqPlain / state.input->sampleRate));
  }

  FBSIMDArray<double, FBFixedBlockSamples> a;
  auto const& gainNorm = procParams.acc.gain[0].Global();
  if (mode == FFGFilterMode::BLL || mode == FFGFilterMode::LSH || mode == FFGFilterMode::HSH)
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      // TODO
      double gainPlain = topo.NormalizedToLinearFast(FFGFilterParam::Gain, gainNorm.CV().Get(s));
      a.Set(s, std::pow(10.0, gainPlain / 40.0));
    }

  if (mode == FFGFilterMode::BLL)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      k.Set(s, k.Get(s) / a.Get(s));
  else if (mode == FFGFilterMode::LSH)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      g.Set(s, g.Get(s) / std::sqrt(a.Get(s)));
  else if (mode == FFGFilterMode::HSH)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      g.Set(s, g.Get(s) * std::sqrt(a.Get(s)));

  FBSIMDArray<double, FBFixedBlockSamples> a1, a2, a3;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    a1.Set(s, 1.0 / (1.0 + g.Get(s) * (g.Get(s) + k.Get(s))));
    a2.Set(s, g.Get(s) * a1.Get(s));
    a3.Set(s, g.Get(s) * a2.Get(s));
  }

  FBSIMDArray<double, FBFixedBlockSamples> m0, m1, m2;
  switch (mode)
  {
  case FFGFilterMode::LPF:
    m0.Fill(0.0);
    m1.Fill(0.0);
    m2.Fill(1.0);
    break;
  case FFGFilterMode::BPF:
    m0.Fill(0.0);
    m1.Fill(1.0);
    m2.Fill(0.0);
    break;
  case FFGFilterMode::HPF:
    m0.Fill(1.0);
    m2.Fill(-1.0);
    for (int s = 0; s < FBFixedBlockSamples; s++)
      m1.Set(s, -k.Get(s));
    break;
  case FFGFilterMode::BSF:
    m0.Fill(1.0);
    m2.Fill(0.0);
    for (int s = 0; s < FBFixedBlockSamples; s++)
      m1.Set(s, -k.Get(s));
    break;
  case FFGFilterMode::PEQ:
    m0.Fill(1.0);
    m2.Fill(-2.0);
    for (int s = 0; s < FBFixedBlockSamples; s++)
      m1.Set(s, -k.Get(s));
    break;
  case FFGFilterMode::APF:
    m0.Fill(1.0);
    m2.Fill(0.0);
    for (int s = 0; s < FBFixedBlockSamples; s++)
      m1.Set(s, -2.0 * k.Get(s));
    break;
  case FFGFilterMode::BLL:
    m0.Fill(1.0);
    m2.Fill(0.0);
    for (int s = 0; s < FBFixedBlockSamples; s++)
      m1.Set(s, k.Get(s) * (a.Get(s) * a.Get(s) - 1.0));
    break;
  case FFGFilterMode::LSH:
    m0.Fill(1.0);
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      m1.Set(s, k.Get(s) * (a.Get(s) - 1.0));
      m2.Set(s, a.Get(s) * a.Get(s) - 1.0);
    }
    break;
  case FFGFilterMode::HSH:
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      m0.Set(s, a.Get(s) * a.Get(s));
      m1.Set(s, k.Get(s) * (1.0 - a.Get(s) * a.Get(s)));
      m2.Set(s, 1.0 - a.Get(s) * a.Get(s));
    }
    break;
  default:
    assert(false);
    break;
  }

  FBSIMDArray2<double, FBFixedBlockSamples, 2> audioIn;
  FBSIMDArray2<double, FBFixedBlockSamples, 2> audioOut = {};
  FBFixedFloatAudioToDoubleArray(input, audioIn);
  
  for (int s = 0; s < FBFixedBlockSamples; s++)
    for (int ch = 0; ch < 2; ch++)
    {
      double v0 = audioIn[ch][s];
      double v3 = v0 - _ic2eq[ch];
      double v1 = a1.Get(s) * _ic1eq[ch] + a2.Get(s) * v3;
      double v2 = _ic2eq[ch] + a2.Get(s) * _ic1eq[ch] + a3.Get(s) * v3;
      _ic1eq[ch] = 2 * v1 - _ic1eq[ch];
      _ic2eq[ch] = 2 * v2 - _ic2eq[ch];
      audioOut[ch][s] = m0.Get(s) * v0 + m1.Get(s) * v1 + m2.Get(s) * v2;
    }

  FBFixedDoubleAudioToFloatArray(audioOut, output);

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