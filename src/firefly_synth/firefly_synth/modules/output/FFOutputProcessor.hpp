#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFOutputProcessor final
{
public:
  void Process(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOutputProcessor);
};