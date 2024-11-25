#pragma once

#include <playground_base/shared/FBHostBlock.hpp>
#include <vector>

class FBInputSplitter
{
  FBFixedInputBlock _fixed;
  FBAccumulatedInputBlock _accumulated;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBInputSplitter);
  FBInputSplitter(int maxHostSampleCount);

  FBFixedInputBlock const* Split();
  void Accumulate(FBHostInputBlock const& input);
};