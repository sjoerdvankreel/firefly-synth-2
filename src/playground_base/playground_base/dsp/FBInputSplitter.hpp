#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBInputSplitter
{
  FBFixedInputBlock _fixed;
  FBAccumulatedInputBlock _accumulated;

public:
  FBInputSplitter(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBInputSplitter);

  FBFixedInputBlock const* Split();
  void Accumulate(FBHostInputBlock const& input);
};