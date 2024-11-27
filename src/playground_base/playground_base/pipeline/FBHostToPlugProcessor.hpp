#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBHostToPlugProcessor
{
  FBFixedInputBlock _fixed = {};
  FBAccumulatedInputBlock _accumulated = {};  

public:
  FBFixedInputBlock const* ToPlug();
  void FromHost(FBHostInputBlock const& host);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToPlugProcessor);
};