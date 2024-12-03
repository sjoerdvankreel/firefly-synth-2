#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBScalarStatePtrs final
{
  std::vector<float*> acc = {};
  std::vector<float*> block = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarStatePtrs);
};