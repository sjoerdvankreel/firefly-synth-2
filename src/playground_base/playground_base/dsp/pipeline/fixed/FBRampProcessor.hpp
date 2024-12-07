#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBFixedInputBlock;
struct FBFixedOutputBlock;

// todo drop this - makes no sense with small blocks + smoothing
class FBRampProcessor final
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBRampProcessor);
  void ProcessRamping(
    FBFixedInputBlock const& input, FBFixedOutputBlock& output);
};