#pragma once

#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniProcessor.hpp>

inline float 
FFGlobalUniProcessor::GetTargetDefault(
  FFGlobalUniTarget target)
{
  switch (target)
  {
  case FFGlobalUniTarget::VMixAmp:
  case FFGlobalUniTarget::OscGain:
    return 1.0f;
  case FFGlobalUniTarget::LFOPhaseOffset:
  case FFGlobalUniTarget::OscPhaseOffset:
    return 0.0f;
  default:
    return 0.5f;
  }
}

inline float 
FFGlobalUniProcessor::GetPhaseOffset(
  FBModuleProcState& state, 
  FFGlobalUniTarget targetParam,
  int voiceSlotInGroup)
{
  if (_voiceCount < 2)
    return 0.0f;

  auto const* procStateContainer = state.input->procState;
  int paramIndex = (int)FFGlobalUniParam::ManualFirst + (int)targetParam;
  auto const* paramTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { paramIndex, voiceSlotInGroup } });
  auto const& cvNorm = procStateContainer->Params()[paramTopo->runtimeParamIndex].GlobalAcc().Global().CV();
  return cvNorm.Get(0);
}

inline void 
FFGlobalUniProcessor::Apply(
  FBModuleProcState& state, FFGlobalUniTarget targetParam,
  int voiceSlotInGroup, FBSArray<float, 16>& targetSignal)
{
  if (_voiceCount < 2)
    return;
  if (_mode[(int)targetParam] == FFGlobalUniMode::Off)
    return;

  FBSArray<float, 16> modSource;
  auto const* procState = state.ProcAs<FFProcState>();
  if (_mode[(int)targetParam] == FFGlobalUniMode::Auto)
  {
    auto const& skew = procState->param.global.globalUni[0].acc.autoSkew[(int)targetParam].Global().CV();
    auto const& spread = procState->param.global.globalUni[0].acc.autoSpread[(int)targetParam].Global().CV();
    auto voicePosBase = FBBatch<float>(voiceSlotInGroup / (_voiceCount - 1.0f));
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      auto voicePos = xsimd::pow(voicePosBase, xsimd::log(skew.Load(s)) * FFInvLogHalf);
      auto outBatch = 0.5f + (voicePos - 0.5f) * spread.Load(s);
      modSource.Store(s, outBatch);
    }
    if (targetParam == FFGlobalUniTarget::VMixAmp || targetParam == FFGlobalUniTarget::OscGain)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        modSource.Store(s, 1.0f - xsimd::abs(FBToBipolar(modSource.Load(s))));
  } else
  {
    auto const* procStateContainer = state.input->procState;
    int manualParamIndex = (int)FFGlobalUniParam::ManualFirst + (int)targetParam;
    auto const* manualParamTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { manualParamIndex, voiceSlotInGroup } });
    auto const& manualCvNorm = procStateContainer->Params()[manualParamTopo->runtimeParamIndex].GlobalAcc().Global().CV();
    manualCvNorm.CopyTo(modSource);
  }

  if (targetParam == FFGlobalUniTarget::VoiceCoarse || targetParam == FFGlobalUniTarget::OscCoarse)
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      auto modAmtSemisNorm = FBToBipolar(modSource.Load(s)) * FFGlobalUniCoarseSemis / FFOsciCoarseSemis * 0.5f;
      targetSignal.Store(s, xsimd::clip(targetSignal.Load(s) + modAmtSemisNorm, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    }
  else if (targetParam == FFGlobalUniTarget::VMixAmp || targetParam == FFGlobalUniTarget::OscGain)
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      targetSignal.Store(s, FFModulateUPMul(modSource.Load(s), 1.0f, targetSignal.Load(s)));
  else
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      targetSignal.Store(s, FFModulateRemap(modSource.Load(s), 1.0f, targetSignal.Load(s)));
}