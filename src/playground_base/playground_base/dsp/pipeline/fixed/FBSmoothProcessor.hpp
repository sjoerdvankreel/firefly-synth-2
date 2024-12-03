#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBFixedOutputBlock;

class FBSmoothProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBSmoothProcessor);
  void ProcessSmoothing(FBFixedOutputBlock& output);
};