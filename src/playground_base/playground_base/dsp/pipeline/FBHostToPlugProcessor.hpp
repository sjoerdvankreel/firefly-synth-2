#pragma once

#include <playground_base/dsp/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/FBPipelineInputBlock.hpp>

struct FBHostInputBlock;

class FBHostToPlugProcessor
{
  FBPlugInputBlock _plug = {};
  FBPipelineInputBlock _pipeline = {};

public:
  FBPlugInputBlock const* ToPlug();
  void FromHost(FBHostInputBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToPlugProcessor);
};