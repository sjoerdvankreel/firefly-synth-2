#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBProcAddrs;
struct FBScalarAddrs;
class FBFixedCVBlock;

struct FBStateAddrs
{
  FBProcAddrs* proc = {};
  FBScalarAddrs* scalar = {};
}; 

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