#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/shared/FBAccEvent.hpp>

#include <vector>

struct FBFixedInputBlock;
struct FBFixedOutputBlock;

class FBSmoothProcessor final
{
  std::vector<FBAccEvent> _accBySampleThenParam = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBSmoothProcessor);
  void ProcessSmoothing(FBFixedInputBlock const& input, FBFixedOutputBlock& output);
};