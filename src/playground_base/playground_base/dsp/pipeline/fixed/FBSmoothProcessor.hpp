#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBFixedOutputBlock;

class FBSmoothProcessor
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBSmoothProcessor);
  void ProcessSmoothing(FBFixedOutputBlock& output);
};