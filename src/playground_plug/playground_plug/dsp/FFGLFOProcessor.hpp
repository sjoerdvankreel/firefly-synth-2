#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFGLFOProcessor
{
  float _phase = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOProcessor);
  void Process(FFModuleProcState const& state);
};