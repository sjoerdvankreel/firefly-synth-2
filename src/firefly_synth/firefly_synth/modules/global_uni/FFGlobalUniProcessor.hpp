#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/osci/FFOsciTopo.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

#include <array>
#include <xsimd/xsimd.hpp>

class FBAccParamState;
struct FBModuleProcState;

class FFGlobalUniProcessor final
{
  int _voiceCount = {};
  FFGlobalUniType _type = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniProcessor);
  void Process(FBModuleProcState& state);
  void BeginBlock(FBModuleProcState& state);

  float ApplyPhase(
    FBModuleProcState& state, 
    FFGlobalUniTarget targetPhaseParam,
    int voice, float targetValue);

  void Apply(
    FBModuleProcState& state, FFGlobalUniTarget targetParam, 
    int voice, FBSArray<float, 16>& targetSignal);
};

inline float 
FFGlobalUniProcessor::ApplyPhase(
  FBModuleProcState& state, 
  FFGlobalUniTarget targetParam,
  int voice, float targetValue)
{
  if (_type == FFGlobalUniType::Off)
    return targetValue;

  FFParkMillerPRNG uniformPrng = {};
  auto const* procStateContainer = state.input->procState;
  int voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;
  int paramIndex = (int)FFGlobalUniParam::FullFirst + (int)targetParam;
  auto const* paramTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { paramIndex, voiceSlotInGroup } });
  auto const& cvNorm = procStateContainer->Params()[paramTopo->runtimeParamIndex].GlobalAcc().Global().CV();
  return FBPhaseWrap(targetValue + cvNorm.Get(s));
}

inline void 
FFGlobalUniProcessor::Apply(
  FBModuleProcState& state, FFGlobalUniTarget targetParam,
  int voice, FBSArray<float, 16>& targetSignal)
{
  if (_type == FFGlobalUniType::Off)
    return;

  auto const* procStateContainer = state.input->procState;
  int voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;
  int paramIndex = (int)FFGlobalUniParam::FullFirst + (int)targetParam;
  auto const* paramTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { paramIndex, voiceSlotInGroup } });
  auto const& cvNorm = procStateContainer->Params()[paramTopo->runtimeParamIndex].GlobalAcc().Global().CV();
  if (targetParam == FFGlobalUniTarget::VoiceCoarse || targetParam == FFGlobalUniTarget::OscCoarse)
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      auto modAmtSemisNorm = FBToBipolar(cvNorm.Load(s)) * FFGlobalUniCoarseSemis / FFOsciCoarseSemis * 0.5f;
      targetSignal.Store(s, xsimd::clip(targetSignal.Load(s) + modAmtSemisNorm, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
    }
  else if(targetParam == FFGlobalUniTarget::VMixAmp || targetParam == FFGlobalUniTarget::OscGain)
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      targetSignal.Store(s, FFModulateUPMul(cvNorm.Load(s), 1.0f, targetSignal.Load(s)));
  else
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      targetSignal.Store(s, FFModulateRemap(cvNorm.Load(s), 1.0f, targetSignal.Load(s)));
}