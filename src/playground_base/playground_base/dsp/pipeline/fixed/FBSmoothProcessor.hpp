#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBFixedInputBlock;
struct FBFixedOutputBlock;

class FBSmoothProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBSmoothProcessor);
  void ProcessSmoothing(FBFixedInputBlock const& input, FBFixedOutputBlock& output);
};