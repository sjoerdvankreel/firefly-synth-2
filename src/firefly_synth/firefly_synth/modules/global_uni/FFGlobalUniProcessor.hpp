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
