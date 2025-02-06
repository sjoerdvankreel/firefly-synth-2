#pragma once

#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FBModuleProcState;

class FFGLFOProcessor final
{
  FBPhase _phase = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOProcessor);
  int Process(FBModuleProcState const& state);
};