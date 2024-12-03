#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

#include <vector>

struct FBProcParamState;

struct FBProcStatePtrs final
{
  std::vector<bool> isBlock = {};
  std::vector<float*> block = {};
  std::vector<FBProcParamState*> acc = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBProcStatePtrs);
};