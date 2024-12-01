#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBScalarAddrs
{
  std::vector<float*> acc = {};
  std::vector<float*> block = {};

  virtual ~FBScalarAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarAddrs);
};