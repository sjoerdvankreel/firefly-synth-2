#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_plug/dsp/shared/FFStateVariableFilter.hpp>

#include <array>

struct FBModuleProcState;

class FFGFilterProcessor final
{
  FFStateVariableFilter<2> _filter = {};

public:
  void Process(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterProcessor);
};