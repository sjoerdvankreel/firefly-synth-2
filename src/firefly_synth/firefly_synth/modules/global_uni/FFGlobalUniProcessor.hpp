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
  std::array<FFGlobalUniMode, (int)FFGlobalUniTarget::Count> _mode = {};
  std::array<bool, (int)FFGlobalUniTarget::Count> _randFree = {};
  std::array<float, (int)FFGlobalUniTarget::Count> _randSeedNorm = {};
  std::array<float, (int)FFGlobalUniTarget::Count> _prevRandSeedNorm = {};
  std::array<FFParkMillerPRNG, (int)FFGlobalUniTarget::Count> _randStream = {};
  std::array<std::array<float, (int)FFGlobalUniTarget::Count>, FBMaxVoices> _voiceRandState = {};

public:
  FFGlobalUniProcessor();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGlobalUniProcessor);

  void BeginVoice(int voice);
  void Process(FBModuleProcState& state);
  void BeginBlock(FBModuleProcState& state);

  float GetTargetDefault(
    FFGlobalUniTarget target);

  float GetPhaseOffsetForVoice(
    FBModuleProcState& state,
    FFGlobalUniTarget targetPhaseParam,
    bool graph, int voice, int voiceSlotInGroup);

  void ApplyToVoice(
    FBModuleProcState& state, FFGlobalUniTarget targetParam, 
    bool graph, int voice, int voiceSlotInGroup,
    FBSArray<float, 16>& targetSignal);
};
