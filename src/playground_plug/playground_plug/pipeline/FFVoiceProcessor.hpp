#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
public:
  bool Process(FBModuleProcState state);
  void BeginVoice(FBModuleProcState state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
};