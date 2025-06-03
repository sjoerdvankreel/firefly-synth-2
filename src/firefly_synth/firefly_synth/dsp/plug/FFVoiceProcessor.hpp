#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
public:
  bool Process(FBModuleProcState state);
  void BeginVoice(FBModuleProcState state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
};