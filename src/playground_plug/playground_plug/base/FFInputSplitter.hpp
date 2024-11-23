#pragma once

#include <playground_plug/base/FFHostBlock.hpp>

class FFInputSplitter
{
  FFFixedInputBlock _fixed;
  FFAccumulatingInputBlock _accumulating;

public:
  FFInputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFInputSplitter);

  FFFixedInputBlock const* NextFixedBlock();
  void AccumulateHostBlock(FFHostInputBlock const& host);
};