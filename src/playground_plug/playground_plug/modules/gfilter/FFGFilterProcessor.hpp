#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FBModuleProcState;

// https://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
class FFGFilterProcessor final
{
  std::array<double, 2> _ic1eq = {};
  std::array<double, 2> _ic2eq = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterProcessor);
  void Process(FBModuleProcState const& state);
};