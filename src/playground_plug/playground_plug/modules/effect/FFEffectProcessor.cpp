#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/effect/FFEffectTopo.hpp>
#include <playground_plug/modules/effect/FFEffectProcessor.hpp>

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

#include <xsimd/xsimd.hpp>

void
FFEffectProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.effect[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Effect];
  auto const& onNorm = params.block.on[0].Voice()[voice];
  _on = topo.NormalizedToBoolFast(FFEffectParam::On, onNorm);
}

void
FFEffectProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceState = procState->dsp.voice[voice];
  auto& output = voiceState.effect[state.moduleSlot].output;
  auto const& input = voiceState.effect[state.moduleSlot].input;

  if (!_on)
  {
    input.CopyTo(output);
    return;
  }

  auto const& procParams = procState->param.voice.effect[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Effect];
  auto const& distDriveNorm = procParams.acc.distDrive[0].Voice()[voice];

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto distDrivePlain = topo.NormalizedToLinearFast(FFEffectParam::ShaperGain, distDriveNorm, s);
    for (int c = 0; c < 2; c++)
      output[c].Store(s, xsimd::tanh(distDrivePlain * input[c].Load(s)));
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->voice[voice].effect[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = -1;

  auto& exchangeParams = exchangeToGUI->param.voice.effect[state.moduleSlot];
  exchangeParams.acc.distDrive[0][voice] = distDriveNorm.Last();
}