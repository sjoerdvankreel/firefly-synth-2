#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_synth/dsp/shared/FFStateVariableFilter.hpp>

#include <array>

struct FBModuleProcState;

class FFGFilterProcessor final
{
  FFStateVariableFilter<2> _filter = {};

public:
  void Process(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterProcessor);
};