#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFMasterProcessor final
{
public:
  void Process(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterProcessor);
};