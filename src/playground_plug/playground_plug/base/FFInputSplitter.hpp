#pragma once

#include <playground_plug/base/FFHostBlock.hpp>

class FFInputSplitter
{
  FFAccumulatingInputBlock _accumulating;

public:
  FFInputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFInputSplitter);

  void RemoveFirstFixedBlock();
  void AccumulateHostBlock(FFHostInputBlock const& host);
  FFAccumulatingInputBlock const* GetFirstFixedBlock() const;
};