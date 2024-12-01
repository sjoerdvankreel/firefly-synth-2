#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class FBFixedInputBlock;
class FBFixedOutputBlock;

class FBRampProcessor
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBRampProcessor);
  void ProcessRamping(
    FBFixedInputBlock const& input, FBFixedOutputBlock& output);
};