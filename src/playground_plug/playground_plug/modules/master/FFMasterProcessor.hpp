#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBModuleProcState;

class FFMasterProcessor final
{
public:
  void Process(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterProcessor);
};