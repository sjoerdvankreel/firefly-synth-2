#pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBInputSplitter
{
  FFFixedInputBlock _fixed;
  FFAccumulatedInputBlock _accumulated;

public:
  FBInputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FBInputSplitter);

  FFFixedInputBlock const* Split();
  void Accumulate(FFHostInputBlock const& input);
};