#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>

struct FBGlobalAccParamState final
{
  float value = 0.0f;
  FBAccParamState proc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBGlobalAccParamState);
};