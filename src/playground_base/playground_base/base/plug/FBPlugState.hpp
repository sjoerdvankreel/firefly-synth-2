#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBProcAddrs;
struct FBScalarAddrs;

class FBFixedCVBlock;
class FBOnePoleFilter;

struct FBStateAddrs
{
  FBProcAddrs* proc = {};
  FBScalarAddrs* scalar = {};
}; 

struct FBProcAddrs
{
  std::vector<int*> pos = {};
  std::vector<FBFixedCVBlock*> cv = {};
  std::vector<FBOnePoleFilter*> smooth = {};

  virtual ~FBProcAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcAddrs);
};

struct FBScalarAddrs
{
  std::vector<float*> acc = {};
  std::vector<float*> block = {};

  virtual ~FBScalarAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarAddrs);
};