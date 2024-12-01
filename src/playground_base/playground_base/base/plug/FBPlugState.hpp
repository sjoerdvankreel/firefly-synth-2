#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

class FBFixedCVBlock;
class FBOnePoleFilter;

struct FBProcStateAddrs;
struct FBScalarStateAddrs;

struct FBStateAddrs
{
  FBProcStateAddrs* proc = {};
  FBScalarStateAddrs* scalar = {};
}; 

struct FBProcStateAddrs
{
  std::vector<int*> pos = {};
  std::vector<FBFixedCVBlock*> cv = {};
  std::vector<FBOnePoleFilter*> smooth = {};

  virtual ~FBProcStateAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcStateAddrs);
};

struct FBScalarStateAddrs
{
  std::vector<float*> acc = {};
  std::vector<float*> block = {};

  virtual ~FBScalarStateAddrs() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBScalarStateAddrs);
};