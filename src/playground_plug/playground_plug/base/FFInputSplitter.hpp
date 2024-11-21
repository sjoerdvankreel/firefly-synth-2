#pragma once

#include <playground_plug/base/FFHostBlock.hpp>

class FFInputSplitter
{
  FFHostInputBlock _accumulatedBlock;

public:
  FFInputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFInputSplitter);

  void RemoveFirstFixedBlock();
  FFHostInputBlock const* GetFirstFixedBlock() const;
  void AccumulateHostBlock(FFHostInputBlock const& hostBlock);
};