#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

#include <vector>

struct FBProcParamState;

struct FBProcStatePtrs final
{
  FBScalarStatePtrs scalar = {};
  std::vector<FBProcParamState*> dense = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBProcStatePtrs);
};