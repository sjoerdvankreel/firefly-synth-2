#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBInputAccumulator
{
  FBFixedInputBlock _fixed;
  FBAccumulatedInputBlock _accumulated;

public:
  FBInputAccumulator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBInputAccumulator);

  bool SplitTo(FBFixedInputBlock const** output);
  void AccumulateFrom(FBHostInputBlock const& input);
};