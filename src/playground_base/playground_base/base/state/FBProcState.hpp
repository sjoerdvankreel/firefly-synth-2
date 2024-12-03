#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBScalarState.hpp>

#include <vector>

struct FBProcParamState;

struct FBProcState
{
  virtual ~FBProcState() {}
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcState);

  FBScalarState scalar = {};
  std::vector<FBProcParamState*> dense = {};
};