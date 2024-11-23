#pragma once

#include <playground_plug/base/shared/FFHostBlock.hpp>

class FFInputSplitter
{
  FFFixedInputBlock _fixed;
  FFAccumulatedInputBlock _accumulated;

public:
  FFInputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFInputSplitter);

  FFFixedInputBlock const* Split();
  void Accumulate(FFHostInputBlock const& host);
};