#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

// https://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
class FFGFilterProcessor
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterProcessor);
  void Process(FFModuleProcState const& state);
};