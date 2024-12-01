#pragma once

#include <playground_base/dsp/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/FBPipelineInputBlock.hpp>

struct FBHostInputBlock;

class FBHostToFixedProcessor
{
  FBFixedInputBlock _fixed = {};
  FBPipelineInputBlock _pipeline = {};

public:
  FBFixedInputBlock const* ToFixed();
  void FromHost(FBHostInputBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToFixedProcessor);
};