#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBProcAccParamState.hpp>

struct FBProcGlobalAccParamState final
{
  float value = 0.0f;
  FBProcAccParamState proc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcGlobalAccParamState);
};