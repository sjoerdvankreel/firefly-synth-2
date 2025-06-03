#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterTopo.hpp>
#include <firefly_synth/modules/gfilter/FFGFilterProcessor.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

void
FFGFilterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto sampleRate = state.input->sampleRate;
  auto& output = procState->dsp.global.gFilter[state.moduleSlot].output;
  auto const& input = procState->dsp.global.gFilter[state.moduleSlot].input;
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GFilter];
  auto const& procParams = procState->param.global.gFilter[state.moduleSlot];
  
  auto const& onNorm = procParams.block.on[0].Value();
  auto const& modeNorm = procParams.block.mode[0].Value();
  bool on = topo.NormalizedToBoolFast(FFGFilterParam::On, onNorm);
  auto mode = topo.NormalizedToListFast<FFStateVariableFilterMode>(FFGFilterParam::Mode, modeNorm);

  if (!on)
  {
    input.CopyTo(output);
    return;
  }

  auto const& resNorm = procParams.acc.res[0].Global();
  auto const& freqNorm = procParams.acc.freq[0].Global();
  auto const& gainNorm = procParams.acc.gain[0].Global();
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    double freqPlain = topo.NormalizedToLog2Fast(FFGFilterParam::Freq, freqNorm.CV().Get(s));
    double resPlain = topo.NormalizedToIdentityFast(FFGFilterParam::Res, resNorm.CV().Get(s));
    double gainPlain = topo.NormalizedToLinearFast(FFGFilterParam::Gain, gainNorm.CV().Get(s));
    _filter.Set(mode, sampleRate, freqPlain, resPlain, gainPlain);
    for (int c = 0; c < 2; c++)
      output[c].Set(s, static_cast<float>(_filter.Next(c, input[c].Get(s))));
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