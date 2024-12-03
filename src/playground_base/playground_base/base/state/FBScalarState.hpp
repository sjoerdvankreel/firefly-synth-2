#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBScalarState
{
  std::vector<float*> acc = {};
  std::vector<float*> block = {};

  virtual ~FBScalarState() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarState);
};