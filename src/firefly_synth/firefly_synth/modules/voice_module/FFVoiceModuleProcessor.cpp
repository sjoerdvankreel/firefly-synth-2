#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/shared/FFStateDetail.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleTopo.hpp>
#include <firefly_synth/modules/voice_module/FFVoiceModuleProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

void
FFVoiceModuleProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto const& procParams = procState->param.voice.voiceModule[0];
  auto const& topo = state.topo->static_->modules[(int)FFModuleType::VoiceModule];

  auto const& fineNormIn = procParams.acc.fine[0].Voice()[voice];
  auto const& coarseNormIn = procParams.acc.coarse[0].Voice()[voice];
  auto const& lfo1ToFine = procParams.acc.lfo1ToFine[0].Voice()[voice];
  auto const& env1ToCoarse = procParams.acc.env1ToCoarse[0].Voice()[voice];
  FBSArray<float, FBFixedBlockSamples> fineNormModulated = {};
  FBSArray<float, FBFixedBlockSamples> coarseNormModulated = {};

  fineNormIn.CV().CopyTo(fineNormModulated);
  coarseNormIn.CV().CopyTo(coarseNormModulated);
  FFApplyModulation(FFModulationOpType::UPStack, voiceState.env[0].output, env1ToCoarse.CV(), coarseNormModulated);
  FFApplyModulation(FFModulationOpType::BPStack, voiceState.vLFO[0].outputAll, lfo1ToFine.CV(), fineNormModulated);

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto coarsePlain = topo.NormalizedToLinearFast(FFVoiceModuleParam::Coarse, coarseNormModulated.Load(s));
    auto finePlain = topo.NormalizedToLinearFast(FFVoiceModuleParam::Fine, fineNormModulated.Load(s));
    voiceState.voiceModule.pitchOffsetInSemis.Store(s, coarsePlain + finePlain);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->voice[voice].voiceModule[0];
  exchangeDSP.active = true;

  auto& exchangeParams = exchangeToGUI->param.voice.voiceModule[0];
  exchangeParams.acc.coarse[0][voice] = coarseNormModulated.Last();
  exchangeParams.acc.env1ToCoarse[0][voice] = env1ToCoarse.Last();
  exchangeParams.acc.fine[0][voice] = fineNormModulated.Last();
  exchangeParams.acc.lfo1ToFine[0][voice] = lfo1ToFine.Last();
}