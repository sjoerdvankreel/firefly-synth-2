#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFGFilterProcessor
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterProcessor);
  void Process(FFModuleProcState const& state);
};