#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBScalarStatePtrs final
{
  std::vector<float*> all = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBScalarStatePtrs);
};