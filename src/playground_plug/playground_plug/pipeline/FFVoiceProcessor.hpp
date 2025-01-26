#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
public:
  void Reset();
  void Process(FFModuleProcState state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
};