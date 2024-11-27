#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBHostToPlugProcessor
{
  FBFixedInputBlock _fixed = {};
  FBPipelineInputBlock _pipeline = {};

public:
  FBFixedInputBlock const* ToPlug();
  void FromHost(FBHostInputBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToPlugProcessor);
};