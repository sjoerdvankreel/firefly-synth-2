#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBInputSplitter
{
  FBFixedInputBlock _fixed;
  FFAccumulatedInputBlock _accumulated;

public:
  FBInputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBInputSplitter);

  FBFixedInputBlock const* Split();
  void Accumulate(FFHostInputBlock const& input);
};