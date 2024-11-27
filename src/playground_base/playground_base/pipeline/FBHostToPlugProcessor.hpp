#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBHostToPlugProcessor
{
  FBFixedInputBlock _fixed = {};
  FBAccumulatedInputBlock _accumulated = {};  

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBHostToPlugProcessor);
  bool SplitTo(FBFixedInputBlock const** output);
  void AccumulateFrom(FBHostInputBlock const& input);
};