#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFEnvProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  void Process(FFModuleProcState const& state, int voice);
};