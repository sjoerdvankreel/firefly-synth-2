#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <vector>

struct FBProcStatePtrs;

struct FBScalarStatePtrs final
{
  std::vector<float*> values = {};
  FB_NOCOPY_MOVE_DEFCTOR(FBScalarStatePtrs);
  
  void InitFrom(FBProcStatePtrs const& proc);
  void InitFrom(FBScalarStatePtrs const& scalar);
};