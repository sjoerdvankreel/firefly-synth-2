#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

class FBFixedCVBlock;

struct FBScalarAddrs
{
  std::vector<float*> acc = {};
  std::vector<float*> block = {};

  virtual ~FBScalarAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarAddrs);
};

struct FBProcAddrs
{
  std::vector<int*> pos = {};
  std::vector<FBFixedCVBlock*> cv = {};

  virtual ~FBProcAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcAddrs);
};