#pragma once

#include <firefly_synth/modules/lfo/FFLFOTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFLFOProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFLFOProcessor);

  int Process(FBModuleProcState& state);
  void BeginVoiceOrBlock(FBModuleProcState& state);
};