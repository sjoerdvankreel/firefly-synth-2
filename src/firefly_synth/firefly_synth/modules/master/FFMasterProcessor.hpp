#pragma once

#include <firefly_synth/modules/master/FFMasterTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <array>

class FBAccParamState;
struct FBModuleProcState;

class FFMasterProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterProcessor);
  void Process(FBModuleProcState& state);
};