#pragma once

#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFGLFOProcessor
{
  FBPhase _phase = {};
  void ProcessOn(FFModuleProcState const& state);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOProcessor);
  void Process(FFModuleProcState const& state);
};