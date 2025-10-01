#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/shared/FFPlugState.hpp>
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
  std::array<int, (int)FFGlobalUniTarget::Count> _randSeed = {};
  std::array<bool, (int)FFGlobalUniTarget::Count> _randFree = {};
  std::array<FFGlobalUniMode, (int)FFGlobalUniTarget::Count> _mode = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniProcessor);
  void Process(FBModuleProcState& state);
  void BeginBlock(FBModuleProcState& state);

  float GetPhaseOffset(
    FBModuleProcState& state, 
    FFGlobalUniTarget targetPhaseParam,
    int voice);

  void Apply(
    FBModuleProcState& state, FFGlobalUniTarget targetParam, 
    int voice, FBSArray<float, 16>& targetSignal);
};

inline float 
FFGlobalUniProcessor::GetPhaseOffset(
  FBModuleProcState& state, 
  FFGlobalUniTarget targetParam,
  int voice)
{
  if (_voiceCount < 2)
    return 0.0f;

  auto const* procStateContainer = state.input->procState;
  int voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;
  int paramIndex = (int)FFGlobalUniParam::ManualFirst + (int)targetParam;
  auto const* paramTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { paramIndex, voiceSlotInGroup } });
  auto const& cvNorm = procStateContainer->Params()[paramTopo->runtimeParamIndex].GlobalAcc().Global().CV();
  return cvNorm.Get(0);
}

inline void 
FFGlobalUniProcessor::Apply(
  FBModuleProcState& state, FFGlobalUniTarget targetParam,
  int voice, FBSArray<float, 16>& targetSignal)
{
  if (_voiceCount < 2)
    return;
  if (_mode[(int)targetParam] == FFGlobalUniMode::Off)
    return;

  auto const* procState = state.ProcAs<FFProcState>();
  auto const* procStateContainer = state.input->procState;
  int voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;

  if (_mode[(int)targetParam] == FFGlobalUniMode::Auto)
  {
    float voicePos = voiceSlotInGroup / (_voiceCount + 1.0f) - 0.5f;
    auto const& spread = procState->param.global.globalUni[0].acc.autoSpread[(int)targetParam].Global().CV();
    for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    {
      FBBatch<float> modSource = 0.5f + voicePos * spread.Load(s);
      targetSignal.Store(s, FFModulateRemap(modSource, 1.0f, targetSignal.Load(s)));
    }
  }
  else
  {
    int manualParamIndex = (int)FFGlobalUniParam::ManualFirst + (int)targetParam;
    auto const* manualParamTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { manualParamIndex, voiceSlotInGroup } });
    auto const& manualCvNorm = procStateContainer->Params()[manualParamTopo->runtimeParamIndex].GlobalAcc().Global().CV();
    if (targetParam == FFGlobalUniTarget::VoiceCoarse || targetParam == FFGlobalUniTarget::OscCoarse)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
      {
        auto modAmtSemisNorm = FBToBipolar(manualCvNorm.Load(s)) * FFGlobalUniCoarseSemis / FFOsciCoarseSemis * 0.5f;
        targetSignal.Store(s, xsimd::clip(targetSignal.Load(s) + modAmtSemisNorm, FBBatch<float>(0.0f), FBBatch<float>(1.0f)));
      }
    else if (targetParam == FFGlobalUniTarget::VMixAmp || targetParam == FFGlobalUniTarget::OscGain)
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        targetSignal.Store(s, FFModulateUPMul(manualCvNorm.Load(s), 1.0f, targetSignal.Load(s)));
    else
      for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
        targetSignal.Store(s, FFModulateRemap(manualCvNorm.Load(s), 1.0f, targetSignal.Load(s)));
  }
}