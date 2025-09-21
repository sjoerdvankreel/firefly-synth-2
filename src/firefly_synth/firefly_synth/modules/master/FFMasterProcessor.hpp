#pragma once

#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFMasterProcessor final
{
  FFMasterUniType _uniType = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterProcessor);
  void Process(FBModuleProcState& state);
  void BeginBlock(FBModuleProcState& state);

  void ApplyGlobalUnison(
    FBModuleProcState& state,
    FFMasterParam uniParam, FFModulationOpType opType, 
    int voice, FBSArray<float, 16>& target);
};

inline void 
FFMasterProcessor::ApplyGlobalUnison(
  FBModuleProcState& state,
  FFMasterParam uniParam, FFModulationOpType opType,
  int voice, FBSArray<float, 16>& target)
{
  // todo not everything is normalized/identity
  FB_ASSERT((int)FFMasterParam::UniFullFirst <= (int)uniParam && (int)uniParam <= (int)FFMasterParam::UniFullLast);

  if (_uniType == FFMasterUniType::Off)
    return;
  
  auto const* procStateContainer = state.input->procState;
  int voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;
  auto const* param = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::Master, 0 }, { (int)uniParam, voiceSlotInGroup } });
  auto const& cvNorm = procStateContainer->Params()[param->runtimeParamIndex].GlobalAcc().Global().CV();
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    target.Store(s, FFModulate(opType, cvNorm.Load(s), FBBatch<float>(1.0f), target.Load(s)));
}