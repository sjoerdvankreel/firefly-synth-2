#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFOsciProcessor
{
  float _phase = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  void Process(FFModuleProcState const& state, int voice);
};