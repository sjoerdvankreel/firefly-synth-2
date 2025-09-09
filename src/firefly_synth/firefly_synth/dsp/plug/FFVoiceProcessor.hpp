#pragma once

#include <firefly_synth/modules/echo/FFEchoTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
  // Need this for on-voice-start modulation.
  bool _firstRoundThisVoice = true;
  FFVEchoTarget GetCurrentVEchoTarget(FBModuleProcState const& state);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
  void BeginVoice(FBModuleProcState state);
  bool Process(FBModuleProcState state, int releaseAt);
};