#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
  void BeginVoice(FBModuleProcState state);
  bool Process(FBModuleProcState state, int releaseAt);
};