#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFGLPFProcessor
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLPFProcessor);
  void Process(FFModuleProcState const& state);
};