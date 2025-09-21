#pragma once

#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/dsp/shared/FFDSPUtility.hpp>
#include <firefly_synth/modules/global_uni/FFGlobalUniTopo.hpp>

#include <firefly_base/dsp/plug/FBPlugBlock.hpp>
#include <firefly_base/base/shared/FBSIMD.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>
#include <firefly_base/base/state/proc/FBProcStateContainer.hpp>

#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFGlobalUniProcessor final
{
  FFGlobalUniType _type = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalUniProcessor);
  void BeginBlock(FBModuleProcState& state);
  void Apply(
    FBModuleProcState& state,
    FFGlobalUniParam param, FFModulationOpType opType, 
    int voice, FBSArray<float, 16>& target);
};

inline void 
FFGlobalUniProcessor::Apply(
  FBModuleProcState& state,
  FFGlobalUniParam param, FFModulationOpType opType,
  int voice, FBSArray<float, 16>& target)
{
  // todo not everything is normalized/identity
  FB_ASSERT((int)FFGlobalUniParam::FullFirst <= (int)uniParam && (int)uniParam <= (int)FFGlobalUniParam::FullLast);

  if (_type == FFGlobalUniType::Off)
    return;
  
  auto const* procStateContainer = state.input->procState;
  int voiceSlotInGroup = state.input->voiceManager->Voices()[voice].slotInGroup;
  auto const* paramTopo = state.topo->audio.ParamAtTopo({ { (int)FFModuleType::GlobalUni, 0 }, { (int)param, voiceSlotInGroup } });
  auto const& cvNorm = procStateContainer->Params()[paramTopo->runtimeParamIndex].GlobalAcc().Global().CV();
  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
    target.Store(s, FFModulate(opType, cvNorm.Load(s), FBBatch<float>(1.0f), target.Load(s)));
}