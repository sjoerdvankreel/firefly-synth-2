#pragma once

#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
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
FFGlobalUniProcessor::GetPhaseOffsetForVoice(
  FBModuleProcState& state, 
  FFGlobalUniTarget targetParam,
  bool graph, int voice, int voiceSlotInGroup)
{
  FB_ASSERT(graph && voice == -1 && voiceSlotInGroup != -1 || !graph && voice != -1 && voiceSlotInGroup == -1);
  FB_ASSERT(targetParam == FFGlobalUniTarget::LFOPhaseOffset || targetParam == FFGlobalUniTarget::OscPhaseOffset);

  if (_voiceCount < 2)
    return 0.0f;

  if (!graph)
    voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;
  else
    voice = voiceSlotInGroup;

  int voiceOffsetInBlock = 0;
  float const phaseShiftMax = 0.99f;
  if (!graph)
    voiceOffsetInBlock = state.input->voiceManager->Voices()[voice].offsetInBlock;
  if (_mode[(int)targetParam] == FFGlobalUniMode::Auto)
  {
    auto const* procState = state.ProcAs<FFProcState>();
    float voicePosBase = voiceSlotInGroup / (_voiceCount - 1.0f);
    auto randOffset = (_voiceRandState[voice][(int)targetParam] - 0.5f) / (_voiceCount - 1.0f);
    auto const& skew = procState->param.global.globalUni[0].acc.autoSkew[(int)targetParam].Global().CV();
    auto const& rand = procState->param.global.globalUni[0].acc.autoRand[(int)targetParam].Global().CV();
    auto const& spread = procState->param.global.globalUni[0].acc.autoSpread[(int)targetParam].Global().CV();
    float voicePos = std::clamp(voicePosBase + rand.Get(voiceOffsetInBlock) * randOffset, 0.0f, 1.0f);
    if (_voiceCount > 3)
      voicePos = FFSkewExpUnipolar(voicePos, skew.Get(voiceOffsetInBlock));
    return voicePos * spread.Get(voiceOffsetInBlock) * phaseShiftMax;
  }
  else
  {
    auto const* procStateContainer = state.input->procState;
    int manualParamIndex = (int)FFGlobalUniParam::ManualFirst + (int)targetParam;
    auto const* manualParamTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { manualParamIndex, voiceSlotInGroup } });
    auto const& manualCvNorm = procStateContainer->Params()[manualParamTopo->runtimeParamIndex].GlobalAcc().Global().CV();
    return manualCvNorm.Get(0) * phaseShiftMax;
  }
}

inline void 
FFGlobalUniProcessor::ApplyToVoice(
  FBModuleProcState& state, FFGlobalUniTarget targetParam,
  bool graph, int voice, int voiceSlotInGroup, FBSArray<float, 16>& targetSignal)
{
  FB_ASSERT(graph && voice == -1 && voiceSlotInGroup != -1 || !graph && voice != -1 && voiceSlotInGroup == -1);
  FB_ASSERT(targetParam != FFGlobalUniTarget::LFOPhaseOffset && targetParam != FFGlobalUniTarget::OscPhaseOffset);

  if (_voiceCount < 2)
    return;
  if (_mode[(int)targetParam] == FFGlobalUniMode::Off)
    return;

  if (!graph)
    voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;
  else
    voice = voiceSlotInGroup;

  FBSArray<float, 16> modSource;
  if (_mode[(int)targetParam] == FFGlobalUniMode::Auto)
  {
    auto const* procState = state.ProcAs<FFProcState>();
    auto voicePosBase = FBBatch<float>(voiceSlotInGroup / (_voiceCount - 1.0f));
    auto randOffset = (_voiceRandState[voice][(int)targetParam] - 0.5f) / (_voiceCount - 1.0f);
    auto const& skew = procState->param.global.globalUni[0].acc.autoSkew[(int)targetParam].Global().CV();
    auto const& rand = procState->param.global.globalUni[0].acc.autoRand[(int)targetParam].Global().CV();
    auto const& spread = procState->param.global.globalUni[0].acc.autoSpread[(int)targetParam].Global().CV();
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      auto voicePos = xsimd::clip(voicePosBase + rand.Load(s) * randOffset, FBBatch<float>(0.0f), FBBatch<float>(1.0f));
      if(_voiceCount > 3)
        voicePos = FFSkewExpBipolar(voicePos, skew.Load(s));
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
      auto modAmtSemisNorm = FBToBipolar(modSource.Load(s)) * 0.5f;
      if (!graph)
        modAmtSemisNorm *= FFGlobalUniCoarseSemis / FFOsciCoarseSemis;
      targetSignal.Store(s, xsimd::clip(targetSignal.Load(s) + modAmtSemisNorm, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    }
  else if (targetParam == FFGlobalUniTarget::VMixAmp || targetParam == FFGlobalUniTarget::OscGain)
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      targetSignal.Store(s, FFModulateUPMul(modSource.Load(s), 1.0f, targetSignal.Load(s)));
  else
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      targetSignal.Store(s, FFModulateRemap(modSource.Load(s), 1.0f, targetSignal.Load(s)));
}