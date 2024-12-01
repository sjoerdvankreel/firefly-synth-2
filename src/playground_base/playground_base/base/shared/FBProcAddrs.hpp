#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

class FBFixedCVBlock;

struct FBProcAddrs
{
  std::vector<int*> pos;
  std::vector<FBFixedCVBlock*> cv;

  virtual ~FBProcAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcAddrs);
};