#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBModuleProcState;

class FFMasterProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterProcessor);
  void Process(FBModuleProcState const& state);
};