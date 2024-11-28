#pragma once

#include <playground_base/shared/FBHostBlock.hpp> // TODO

class FBHostToPlugProcessor
{
  FBPlugInputBlock _plug = {};
  FBPipelineInputBlock _pipeline = {};

public:
  FBPlugInputBlock const* ToPlug();
  void FromHost(FBHostInputBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToPlugProcessor);
};