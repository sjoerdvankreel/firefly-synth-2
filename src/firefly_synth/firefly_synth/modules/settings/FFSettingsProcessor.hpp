#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

struct FBModuleProcState;

class FFSettingsProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFSettingsProcessor);
  void Process(FBModuleProcState& state);
};