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
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterProcessor);
  void Process(FBModuleProcState& state);
};